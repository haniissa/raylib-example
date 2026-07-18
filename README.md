# 3D Cube Arena Shooter

A fast-paced 3D arena shooter built with **C++20**, **raylib** , and **GLSL shaders** , with an exerimental  **Vulkan compute** pipeline , Built as a learning project with **AI** assistence to practice game development and graphics programming convepts.




## Features
- **Third-person 3D camera** with WASD movement and mouse-look
-   **Spacebar  shooting** with 0.18s cooldown and pool-based bullet managment (64 bullets)
-- **Enemy AI ** - 8-12 cubes that bounce off arena walls with random colors, size, and velocities
-- **AABB cikkusuib detectuib** - bullets vs enemy, player vs enemy 
-- **Custom GLSL shaders** - vertex/fragment pipeline, lose a life on contact, respawn at center
-- **Vulkan compute** (experimental) - headless compute pipeline using vk-bootstrap (WIP)
-- **HUD overlay** - FPS counter, cour/lives display, control hints

## Tech Stack 
| Component | Details | 
|-----------|---------|
| Language  | C++20   |
| Graphics  | raylib 5.5 + OpenGL 3.3 | 
| Shaders   | GLSL 330 (vertex/fragment), GLSL 460 (compute) |
| compute | Vulkan 1.3 + vk-bootstrap |
| Formatting | fmt 11.0 |
| Build | Cmake + Ninja (MinGW64) |


## Build & Run 

## Prerequisites 
- MSYS2 with MinGW64 toolchain 
- raylib, fmt, Vulkan SDK, vk-bootstrap installed via `pacman` 

### Commands 
```bash
    cmake -G Ninja -D PROJECT_NAME="NAME.exe" -D CMAKE_PREFIX_PATH="C:/msys64/mingw64" -B build -S ./
    cmake --build build  --verbose --config Release
    ./build/NAME.exe 
```

## Controls 
| KEY  | Action |
|------|--------|
|W/A/S/D| Move player |
|Space | Shoot |
| Mouse | Look around (third-person camera) |
|ESC | Exit |

## What I Learned 
- **Game architecture** --- separating Player, Enemy, Bullet, and Game logic into clean classes 
- **Object pooling** --resusing bullet slots instead of allocting/deallocating every frame 
- **AABB collision detection** --- axis-aligned bounding box overlap test 
- **GLSL shader programing** ---writing vertex/fragment shaders with time-based uniforms 
- **Vulkan compute basics** -- instance creation, device selection, command buffers, pipeline setup 
- **CMake build system** --- FetchContent fallbacks, cross-platform dependency management  
- **C++20 feature** --- designated initializers, std::clamp, structured bindings


## Acknowledgments 
Built with AI assistance as a practice project for portfolio and job preparation, Every line was written, reviewed, and understood -- Ai was as a learning tool, not a shortcut
