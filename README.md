# Reverse Engineering Proof of Concept — Sauerbraten

Proof-of-concept DLL developed as part of my undergraduate thesis on **using generative AI to assist in reverse engineering**. The target application is [Sauerbraten (Cube 2)](http://sauerbraten.org/), an open-source first-person shooter, chosen specifically because its source code is publicly available and can be studied offline.

## What It Does

The DLL, once injected into the Sauerbraten process, demonstrates functionalities based on the reverse-engineered findings:

| Feature | Description |
|---------|-------------|
| **Aimbot** | Calculates yaw/pitch angles to the closest visible enemy and writes them to the local player's view angles. |
| **ESP (Extra-Sensory Perception)** | Draws 2D corner boxes around players by hooking `SDL_GL_SwapWindow` and rendering via OpenGL. Box color indicates team (blue), visible enemy (green), or occluded enemy (red). A vertical health bar is drawn beside each box. |
| **Visibility Check** | Uses the engine's own `raycube` function to determine line-of-sight between the local player and each target. |

### Controls

| Key | Action |
|-----|--------|
| `Numpad 1` | Toggle aimbot on/off |
| `Numpad 0` | Eject DLL and clean up |

## Project Structure

```
ProofOfConcept/
├── ProofOfConcept.sln              # Visual Studio solution (2 projects)
├── dependencies/
│   ├── include/
│   │   └── MinHook/MinHook.h       # MinHook hooking library
│   └── lib/
│       └── libMinHook.x64.lib
├── Injector/
│   ├── Injector.vcxproj            # Console app — DLL injector
│   └── Injector.cpp                # CreateRemoteThread + LoadLibraryA injection
└── ProofOfConcept/
    ├── ProofOfConcept.vcxproj      # DLL project — the PoC payload
    ├── dllmain.cpp                 # DLL entry point & aimbot logic
    ├── hooks.cpp / hooks.h         # SDL_GL_SwapWindow hook, ESP rendering, raycube visibility
    ├── sdk.cpp / sdk.h             # Initialization (module base, entity list, matrices)
    ├── Player.h                    # Reverse-engineered player struct layout
    ├── math.h                      # Vector2/3/4, Matrix4x4 types
    └── offsets.h                   # Memory offsets for entity list & lobby size
```

## Dependencies

| Dependency | Purpose | Included |
|------------|---------|----------|
| [MinHook](https://github.com/tsudakageyu/minhook) | API hooking (used to hook `SDL_GL_SwapWindow`) | Yes (`dependencies/`) |

## Build Instructions

### Prerequisites

- **Visual Studio**  with the **C++** workload
- **Windows SDK 10.0**
- **C++ Language Standard: C++20 or later** (required for the DLL project — the code uses C++20 features)

### Steps

1. Clone the repository:
   ```
   git clone https://github.com/nicklouko/Thesis-ProofOfConcept
   ```
2. Open `ProofOfConcept.sln` in Visual Studio 2022.
3. Select configuration **Release | x64** (or **Debug | x64**).
4. Verify the C++ Language Standard for the **ProofOfConcept** (DLL) project is set to **ISO C++20** or later:
   - Right-click the project → Properties → C/C++ → Language → **C++ Language Standard** → `ISO C++20 (/std:c++20)`.
5. **Update the DLL path** in `Injector/Injector.cpp` (line 28) to the **absolute path** where `ProofOfConcept.dll` will be built on your machine (e.g. `C:\...\x64\Release\ProofOfConcept.dll`).
6. Build the entire solution (`Ctrl+Shift+B`). This compiles both projects:
   - **ProofOfConcept** → `x64/Release/ProofOfConcept.dll` (the payload DLL)
   - **Injector** → `x64/Release/Injector.exe` (the injector console app)

> **Note:** Both projects target **x64** only. Sauerbraten ships as a 64-bit executable on modern Windows builds. The Injector project uses **Multi-Byte** character set; the DLL project uses **Unicode** — these are already configured in the `.vcxproj` files.

## How to Run

1. Launch **Sauerbraten** and start a local game with bots (offline/singleplayer).
2. Run `Injector.exe` **as Administrator**. It will find the game window by title (`Cube 2: Sauerbraten`) and inject the DLL automatically.

> **Troubleshooting:** If the Injector window opens and closes instantly without output, the DLL path in `Injector.cpp` is most likely incorrect. Double-check that the path on line 28 points to the actual location of `ProofOfConcept.dll` and rebuild.
3. A console window will open inside the game process with initialization logs.
4. Make sure **Num Lock is enabled** on your keyboard.
5. Use `Numpad 1` to toggle the aimbot; ESP renders automatically.
6. Press `Numpad 0` to eject the DLL cleanly.

## Tested Environment

| Component | Version |
|-----------|---------|
| OS | Windows 11 Pro (10.0.22631) |
| IDE | Visual Studio 2022 |
| C++ Version | **C++20** (`/std:c++20`) |
| Target | Sauerbraten  |
| Architecture | x64 |

## Ethical & Legal Notice

This project is developed **exclusively for academic research purposes** as part of a university thesis on AI-assisted reverse engineering.

- **Target selection:** Sauerbraten is an [open-source game](http://sauerbraten.org/) released under the Zlib license. It was chosen deliberately as a permissive, offline-capable research target.
- **Offline use only:** This PoC is intended to run against local/offline game sessions with bots. It is **not** designed, intended, or suitable for use on public multiplayer servers.
- **No malicious intent:** The code demonstrates offensive techniques (memory layout analysis, function hooking, rendering interception) for educational purposes. It is not a cheating tool for competitive play.
- **Thesis context:** The reverse engineering process documented in the thesis explores how generative AI models can accelerate tasks such as identifying data structures, locating function signatures and understanding memory layouts in compiled binaries.

**Do not use this software to gain unfair advantages in online games or to violate any terms of service.**
