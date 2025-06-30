Members:	Marcus Leocario
    		Joachim Arguelles

# Elden’s Hill — OpenGL Graphics Project

**Elden’s Hill** is a 3D exploratory game prototype built with **OpenGL**, **C++**, **GLFW**, and **tinyobjloader**, designed as a night patrol mission turned surreal mystery. The player controls a tank transported into a strange flatland under the moonlight, where they must explore the terrain and search for clues — all while navigating realistic lighting, multiple camera perspectives, and custom 3D models.

## Gameplay Overview

- Take control of a **fully textured and normal-mapped tank** model.
- Explore a mysterious night-time environment scattered with **6+ interactive 3D models**.
- Navigate through a large textured terrain that gives the illusion of infinite space.
- Use multiple camera modes for different playstyles and perspectives:
  - **3rd-Person View** for navigation
  - **1st-Person Night Vision Binoculars** for scouting far-off objects
  - **Orthographic Top-Down View** for full map awareness

## Key Features

- **3D Models**: Includes at least 7 external 3D models with full texture and normal mapping support.
- **Skybox**: A custom night sky skybox surrounds the player’s world.
- **Lighting System**: 
  - Directional moonlight
  - Adjustable tank-mounted point light (Low, Medium, High via `F` key)
- **Cameras**:
  - Switch between perspectives using `1` and `2` keys
  - Night vision in first-person mode simulates single-color tone visibility
  - Orthographic camera supports WASD panning
- **Player Controls**: Fully responsive tank and binocular movement using `WASDQE`.
- **Object-Oriented Architecture**: Modular classes for models, lighting, cameras, shaders, and the player.

## Tech Stack

- **Language**: C++
- **Graphics API**: OpenGL
- **Libraries**: GLFW, tinyobjloader
- **File Handling**: External OBJ, PNG, and normal map files for model and environment assets

## Project Structure

- `Model`, `Light`, `Camera`, `Player`, and `Shader` classes are separated and organized.
- Input handled via GLFW callback functions in `main.cpp`.

## Assets Used

- Models sourced from TurboSquid, Free3D, and Sketchfab (e.g., Tiger I tank, fictional tanks, environmental props).
- Skybox textures sourced from [pngwing.com](https://www.pngwing.com/en/free-png-hzcii).
