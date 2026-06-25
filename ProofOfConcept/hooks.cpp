#include "hooks.h"
#include <Windows.h>
#include <gl/GL.H>
#include <MinHook/Minhook.h>
#include <iostream>
#include "math.h"
#include "Player.h"




Vector2 WorldToScreen(Vector3 worldPos, Matrix4x4* viewMatrix, Matrix4x4* projectionMatrix, GLint viewport[4]);
bool hooks::install()
{
	if (is_initialized)
		return true;

	HMODULE h_sdl2{ GetModuleHandleA("SDL2.dll") };

	if (MH_Initialize() != MH_OK) {
		std::cout << "[INFO] MinHook initialization failed!" << std::endl;
	}
	else {
		std::cout << "[INFO] MinHook initialized succesfully!\n";
	}

	if (h_sdl2 == INVALID_HANDLE_VALUE) {
		std::cout << "[INFO] Failed to get handle to SDL2.DLL\n";
	}
	else {
		std::cout << "[INFO] Handle to SDL2.dll: " << h_sdl2 << "\n";
	}

	p_sdl_swap_window = (void*)GetProcAddress(h_sdl2, "SDL_GL_SwapWindow");

	if (p_sdl_swap_window == nullptr) {
		std::cout << "[INFO] Failed to get procAddress to SDL2.DLL\n";
	}
	else {
		std::cout << "[INFO] Address of SDL_GL_SwapWindow: " << p_sdl_swap_window << "\n";
	}

	// Create the hook (hook SDL_GL_SwapWindow to SwapWindowHook)
	if (MH_CreateHook(p_sdl_swap_window, &SwapWindow, reinterpret_cast<LPVOID*>(&p_original_swap_window)) != MH_OK) {
		std::cout << "Failed to create hook for SDL_GL_SwapWindow" << std::endl;
		return false;
	}

	// Enable the hook
	if (MH_EnableHook(p_sdl_swap_window) != MH_OK) {
		std::cout << "Failed to enable hook for SDL_GL_SwapWindow" << std::endl;
		return false;
	}

	std::cout << "Hook installed successfully!" << std::endl;

    raycube = reinterpret_cast<raycube_t>(sdk::base+0x1142C0);
    std::cout << "base + offset is at 0x" << std::hex << raycube << std::endl;
	
	is_initialized = true;
	return true;
}
void hooks::SwapWindow(void* p_window)
{
    HDC hdc{ wglGetCurrentDC() };
    HGLRC current_context{ wglGetCurrentContext() };
    static HGLRC custom_context{};

    if (!custom_context)
    {
        custom_context = wglCreateContext(hdc);
        if (!wglMakeCurrent(hdc, custom_context))
        {
            std::cout << "Can't create and set custom OpenGL context.\n";
            wglDeleteContext(custom_context);
            return p_original_swap_window(p_window);
        }

        // Get the game's current viewport
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);  // Get the current viewport dimensions
        glViewport(0, 0, viewport[2], viewport[3]);  // Set the same viewport for  custom context

        // Set up orthogonal projection matrix for 2D rendering
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, viewport[2], viewport[3], 0, -1, 1); // Left, Right, Bottom, Top

        // Switch to the modelview matrix mode and disable depth testing
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        std::cout << "Custom OpenGL context created and projection set.\n";
    }
    else
    {
        if (!wglMakeCurrent(hdc, custom_context))
        {
            std::cout << "Can't make custom context current.\n";
            wglDeleteContext(custom_context);
            return p_original_swap_window(p_window);
        }

        // Get the viewport here, so it's valid when calling WorldToScreen
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);  // Get the current viewport dimensions

        
        
       
        uintptr_t* entityList = *reinterpret_cast<uintptr_t**>(sdk::base + 0x346C90);

        for (int i = 1; i < sdk::lobbySize; ++i) {

            Player* player = reinterpret_cast<Player*>(entityList[i]);
            
            if (!player) continue;
            

            if (player->isDead) continue;

            // Bottom and top positions in world space
            Vector3 worldBottom = { player->xpos, player->ypos, player->zpos - 16.0f };
            Vector3 worldTop = { player->xpos, player->ypos, player->zpos + 2.0f }; // approximate player height

            // Project to screen space
            Vector2 screenBottom = WorldToScreen(worldBottom, sdk::viewMatrix, sdk::ProjectionMatrix, viewport);
            Vector2 screenTop = WorldToScreen(worldTop, sdk::viewMatrix, sdk::ProjectionMatrix, viewport);

            // Visibility check
            if (screenBottom.x == -1.0f || screenBottom.y == -1.0f) continue;
            if (screenTop.x == -1.0f || screenTop.y == -1.0f) continue;

            // Using IsVisible for actual line-of-sight check
            Player* localPlayer = reinterpret_cast<Player*>(entityList[0]);
            vec localPlayerPos = { localPlayer->xpos, localPlayer->ypos, localPlayer->zpos };
            vec target = { player->xpos, player->ypos, player->zpos };

            bool visible = hooks::IsVisible(localPlayerPos, target);  // Check visibility
            bool myteam = !(std::memcmp(localPlayer->team, player->team, 4));
                

            // Draw snapline from bottom center of screen to bottom of player
            //glBegin(GL_LINES);
            //glColor3f(1.0f, 0.0f, 0.0f);  // Red for the snapline
            //glVertex2f(viewport[2] / 2.0f, viewport[3]);  // screen bottom center
            //glVertex2f(screenBottom.x, screenBottom.y);
            //glEnd();

            // Calculate box dimensions
            float height = screenBottom.y - screenTop.y;
            float width = height / 2.0f;

            float left = screenTop.x - width / 1.8f;
            float right = screenTop.x + width / 1.8f;
            float top = screenTop.y;
            float bottom = screenBottom.y;

            // Draw 2D box (color changes based on visibility)
            float lineW = (right - left) * 0.25f;  // Width of corner lines (25% of box width)
            float lineH = (bottom - top) * 0.25f;  // Height of corner lines (25% of box height)

            glBegin(GL_LINES);
            if (myteam) {
                glColor3f(0.0f, 0.0f, 1.0f);

            }
            else {
                if (visible) {
                    glColor3f(0.0f, 1.0f, 0.0f);  // Green if visible
                }
                else {
                    glColor3f(1.0f, 0.0f, 0.0f);  // Red if not visible
                }
            }
            // Top Left corner
            glVertex2f(left, top);
            glVertex2f(left + lineW, top);

            glVertex2f(left, top);
            glVertex2f(left, top + lineH);

            // Top Right corner
            glVertex2f(right, top);
            glVertex2f(right - lineW, top);

            glVertex2f(right, top);
            glVertex2f(right, top + lineH);

            // Bottom Left corner
            glVertex2f(left, bottom);
            glVertex2f(left + lineW, bottom);

            glVertex2f(left, bottom);
            glVertex2f(left, bottom - lineH);

            // Bottom Right corner
            glVertex2f(right, bottom);
            glVertex2f(right - lineW, bottom);

            glVertex2f(right, bottom);
            glVertex2f(right, bottom - lineH);

            glEnd();


            // Draw a vertical health bar on the left side of the player box
            float healthPercentage = static_cast<float>(player->Health) / 100.0f; 
            float barHeight = screenBottom.y - screenTop.y;
            float healthBarHeight = barHeight * healthPercentage;

            // Calculate the left position for the health bar 
            float healthBarX = left - 7.0f; // Slightly to the left of the player box (with extra gap)
            float healthBarY = top + 1.0f;  // Small gap from the top of the player box

            // Draw the health bar background (gray or black)
            glBegin(GL_QUADS);
            glColor3f(0.2f, 0.2f, 0.2f);  // Dark gray background for the bar
            glVertex2f(healthBarX, healthBarY);            // Top left of the bar
            glVertex2f(healthBarX + 4.0f, healthBarY);    // Top right of the bar
            glVertex2f(healthBarX + 4.0f, healthBarY + barHeight);  // Bottom right of the bar
            glVertex2f(healthBarX, healthBarY + barHeight);          // Bottom left of the bar
            glEnd();

            // Draw the health bar (color changes based on health percentage)
            glBegin(GL_QUADS);
            if (healthPercentage > 0.5f) {
                glColor3f(0.0f, 1.0f, 0.0f);  // Green for high health
            }
            else if (healthPercentage > 0.2f) {
                glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for medium health
            }
            else {
                glColor3f(1.0f, 0.0f, 0.0f);  // Red for low health
            }

            // Draw the filled health bar (top to bottom)
            glVertex2f(healthBarX, healthBarY + barHeight - healthBarHeight);  // Start from top
            glVertex2f(healthBarX + 4.0f, healthBarY + barHeight - healthBarHeight);  // Start from top
            glVertex2f(healthBarX + 4.0f, healthBarY + barHeight);  // End at the bottom of the bar
            glVertex2f(healthBarX, healthBarY + barHeight);  // End at the bottom of the bar
            glEnd();
        }
    }

    // Restore the original OpenGL context
    wglMakeCurrent(hdc, current_context);

    // Call the original swap window function to swap buffers
    return p_original_swap_window(p_window);
}




void hooks::remove()
{
	if (!is_initialized)
		return;

	if (MH_DisableHook(p_original_swap_window) != MH_OK) {
		std::cout << "Failed to disable hook!" << std::endl;
	}

	// Uninitialize MinHook
	if (MH_Uninitialize() != MH_OK) {
		std::cout << "Failed to uninitialize MinHook!" << std::endl;
	}

	is_initialized = false;
}

Vector2 WorldToScreen(Vector3 worldPos, Matrix4x4* viewMatrix, Matrix4x4* projectionMatrix, GLint viewport[4])
{
    // Convert world position (Vector3) to camera space (Vector4)
    Vector4 cameraSpace = Vector4(worldPos.x, worldPos.y, worldPos.z, 1.0f).mulColumnMajor4x4Matrix(viewMatrix);

    // Apply the projection matrix to the camera space position
    Vector4 ndc = cameraSpace.mulColumnMajor4x4Matrix(projectionMatrix);

    // Normalize 
    if (ndc.w != 0.f)
    {
        ndc.x /= ndc.w;
        ndc.y /= ndc.w;
        ndc.z /= ndc.w;
    }

    //  Check if player is within view 
    // In NDC space, values should be between -1 and 1 for both x and y
    if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f || ndc.z < 0.0f || ndc.z > 1.0f) {
        return Vector2(-1.0f, -1.0f);  // Return invalid screen position if outside view
    }

    // Step 5: Map to screen space (2D coordinates)
    Vector2 screenPos;
    screenPos.x = (ndc.x + 1.0f) * 0.5f * viewport[2] + viewport[0];  // X screen space
    screenPos.y = (1.0f - ndc.y) * 0.5f * viewport[3] + viewport[1];  // Y screen space (flip Y)

    return screenPos;  // Return the 2D screen position
}



bool hooks::IsVisible(const vec& from, const vec& to)
{
    vec direction = to - from;
    float totalDistance = direction.length();
    if (totalDistance == 0.0f) return false;

    vec normalizedDir = direction / totalDistance;

    
    float hitDist = hooks::raycube(&from, &normalizedDir, totalDistance, 0x01, 0, nullptr);

    return hitDist >= totalDistance - 0.01f;
}
