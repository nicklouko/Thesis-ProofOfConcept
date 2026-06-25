#include <Windows.h>
#include <iostream>
#include <thread>
#include <cmath>
#include <cstdio>
#include <fstream>
#include "sdk.h"
#include "hooks.h"
#include <algorithm>



#define M_PI 3.14159265358979323846

const float FOV = 90.0f; // Adjust this to match the game's FOV
const float YAW_OFFSET = 90.0f; // Adjust if needed



bool aimbotEnabled = false;

void injected_thread(HMODULE hModule) {
    do {
        if (!sdk::initialize())
            break;
        std::cout << "[INFO] DLL injected successfully.\n";
        if (!hooks::install())
            break;

        
        

        while (true) {  // Main loop, will run until Numpad0 is pressed
            sdk::lobbySize = *reinterpret_cast<int*>(sdk::base + 0x346C9C);
            Player* localPlayer = reinterpret_cast<Player*>(sdk::entityList[0]);
            uintptr_t* entityList = *reinterpret_cast<uintptr_t**>(sdk::base + 0x346C90);

            if (GetAsyncKeyState(VK_NUMPAD0) & 1) {  // Check if Numpad0 was pressed to exit
                break;
            }

            if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) {  // Toggle aimbot
                aimbotEnabled = !aimbotEnabled;
                std::cout << "Aimbot " << (aimbotEnabled ? "enabled" : "disabled") << std::endl;
                
                
                Sleep(200);  
            }

            if (!aimbotEnabled) continue;

            if (!localPlayer || localPlayer->isDead) continue;

            Player* closestEnemy = nullptr;
            float closestDistance = FLT_MAX;

            

            for (int i = 1; i < sdk::lobbySize; ++i) {  // start at 1 to skip local player
                Player* player = reinterpret_cast<Player*>(entityList[i]);
                if (!player || player->isDead) continue;
                if (std::memcmp(localPlayer->team, player->team, 4) == 0) {
                    continue;
                }
                vec localPlayerPos = { localPlayer->xpos, localPlayer->ypos, localPlayer->zpos };
                vec target = { player->xpos, player->ypos, player->zpos };

                bool visible = hooks::IsVisible(localPlayerPos, target);
                if (!visible) continue;

                float dx = player->xpos - localPlayer->xpos;
                float dy = player->ypos - localPlayer->ypos;
                float dz = player->zpos - localPlayer->zpos;

                float distance = sqrtf(dx * dx + dy * dy + dz * dz);

                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestEnemy = player;
                }
            }

            if (closestEnemy) {
                float dx = closestEnemy->xpos - localPlayer->xpos;
                float dy = closestEnemy->ypos - localPlayer->ypos;
                float dz = closestEnemy->zpos - localPlayer->zpos;

                float yaw = atan2f(dy, dx) * (180.0f / static_cast<float>(M_PI));
                yaw = fmodf(yaw + 360.0f, 360.0f);  // normalize to [0, 360)

                float horizontal_distance = sqrtf(dx * dx + dy * dy);
                float pitch = atan2f(dz, horizontal_distance) * (180.0f / static_cast<float>(M_PI));
                pitch = std::clamp(pitch, -90.0f, 90.0f);

                localPlayer->Yaw = fmodf(yaw - 90.0f + 360.0f, 360.0f); // adjust for engine yaw offset
                localPlayer->Pitch = pitch;
            }

            Sleep(10);  // Reduce CPU usage while waiting for key state or game state updates
        }

        std::cout << "[INFO] Exiting injected thread.\n";

    } while (false);


    hooks::remove();
    

    FreeLibraryAndExitThread(hModule, 0);
   
        
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    static FILE* f = nullptr;


    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        AllocConsole();
        freopen_s(&f, "CONOUT$", "w", stdout);
        HANDLE h_thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(injected_thread), hModule, 0, 0);
        CloseHandle(h_thread);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        fclose(f);
        FreeConsole();

    }

  
    return TRUE;
}