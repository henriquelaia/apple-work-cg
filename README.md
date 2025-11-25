# Apple CG Assignment

This project renders a 3D Apple model with dynamic lighting, textures, and interactive controls using OpenGL (GLFW, GLEW, GLM).

## Features

*   **3D Model Loading**: Loads an `Apple.obj` model with textures.
*   **Dynamic Lighting**:
    *   Point light source with adjustable position and color.
    *   **Flashlight Mode**: A spotlight that follows the camera.
    *   **Two-Sided Lighting**: Correctly lights the leaf from both sides.
*   **Interactive Controls**:
    *   Camera movement (WASD + Mouse).
    *   Light source control (Arrows).
    *   **Auto-Rotation**: Toggle automatic rotation of the apple.
    *   **Random Colors**: Change light color to random values.
*   **Texture Handling**: Supports transparency (alpha testing) for the leaf.

## Prerequisites

*   **CMake**
*   **C++ Compiler** (g++, clang, or MSVC)
*   **OpenGL Libraries**:
    *   GLFW
    *   GLEW
    *   GLM

## Build Instructions

1.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Generate build files with CMake:**
    ```bash
    cmake ..
    ```

3.  **Compile the project:**
    ```bash
    make
    ```

4.  **Run the application:**
    ```bash
    ./app
    ```

## Controls

### Camera
*   **`W`, `A`, `S`, `D`**: Move Camera
*   **Mouse**: Look around

### Light Controls
*   **`Arrow Keys`**: Move/Rotate the light source (when Flashlight is OFF)
*   **`1`**: White Light
*   **`2`**: Red Light
*   **`3`**: Green Light
*   **`4`**: Blue Light
*   **`5`**: Yellow Light
*   **`Space`**: **Random Light Color**

### Special Effects
*   **`F`**: Toggle **Flashlight Mode** (Spotlight follows camera)
*   **`R`**: Toggle **Auto-Rotation** (Apple spins automatically)

## Project Structure

*   `src/`: Source code (`main.cpp`) and assets (`Apple.obj`, `apple_color.png`).
*   `shaders/`: GLSL shaders (`2.1.basic_lighting.vs`, `2.1.basic_lighting.fs`, etc.).
*   `common/`: Helper headers (`objloader.hpp`, `shader_m.h`, `camera.h`).
*   `CMakeLists.txt`: Build configuration.
