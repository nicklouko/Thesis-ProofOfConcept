#include <Windows.h>
#include "sdk.h"
#include <iostream>
#include "gl/GL.h"



bool sdk::initialize()
{
	if (is_Initialized)
		return true;


    base = reinterpret_cast<uintptr_t>(GetModuleHandleA("sauerbraten.exe"));
    
    if (base == 0) {
        std::cout << "[ERROR] Failed to get module base address\n";
        
    }
    std::cout << "[INFO] Module base: 0x" << std::hex << base << std::dec << "\n";

    //  Get the entity list
   
    entityList = *reinterpret_cast<uintptr_t**>(base + 0x346C90);
    if (entityList == nullptr) {
        std::cout << "[ERROR] Failed to get entity list\n";
        
    }
    lobbySize = *reinterpret_cast<int*>(base + 0x346C9C);
    
   
    std::cout << "[INFO] Entity list: 0x" << std::hex << (uintptr_t)entityList << std::dec << "\n";
    std::cout << "[INFO] Lobby size: " << lobbySize << std::endl;

    viewMatrix = reinterpret_cast<Matrix4x4*>(base + 0x32CF94 + 0x2C);
    ProjectionMatrix = reinterpret_cast<Matrix4x4*>(base + 0x32CF94 + 0x6C);
    viewProjectionMatrix = reinterpret_cast<Matrix4x4*>(base + 0x32CF94 + 0xAC);
    

    if (viewMatrix)
    {
        std::cout << "ViewProjectionMatrix:\n";
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                std::cout << ProjectionMatrix->access(i, j) << " ";
            }
            std::cout << "\n";
        }
    }
    else
    {
        std::cout << "viewProjectionMatrix is null.\n";
    }

 

    is_Initialized = true;
    return is_Initialized;

    //  Get local player
    Player* localPlayer = reinterpret_cast<Player*>(entityList[0]);
    if (localPlayer == nullptr) {
        std::cout << "[ERROR] Failed to get local player\n";

    }



	
}

void sdk::drawline(Vector2 from, Vector2 to, float r, float g, float b, float thickness)
{
    glLineWidth(thickness);
    glBegin(GL_LINES);
    glColor4f(r, g, b, 1.0f);
    glVertex2f(from.x, from.y);
    glVertex2f(to.x, to.y);
    glEnd();
    glLineWidth(1.0f);


}