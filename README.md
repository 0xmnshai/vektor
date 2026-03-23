# Vektor ( PAUSED WORKING ON THIS PROJECT, FOR SOME TIME)

A hybrid C++20/Rust real-time engine and DCC editor built for performance and modularity. Vektor leverages a Blender-inspired DNA/RNA data model, a Rust-accelerated compute pipeline, and a dual-backend (OpenGL/Metal) Hardware Abstraction Layer.

## Tech Stack
* **Languages:** C++20 (Core Engine), Rust (Compute/SIMD), Python (Planned Scripting).
* **Editor:** Qt6 with a custom docking system and INI-driven UI styling.
* **Graphics:** Hardware Abstraction Layer (HAL) supporting **OpenGL 4.1** and **Apple Metal**.
* **Shaders:** **Slang** for cross-compilation and unified shader management.
* **ECS:** **EnTT** for entity management and system orchestration.
* **Math:** GLM (C++) and a custom Rust `math_accel` crate using `wide` (SIMD) and `rayon` (Parallelism).
* **Memory:** Custom guarded allocator and a 16MB atomic bump (frame) allocator for zero-allocation transient data.

## Architecture

### DNA & RNA (The Data Spine)
The engine uses a strict separation between data and reflection:
* **DNA:** Header-only POD (Plain Old Data) structs defining the engine's state. This ensures memory layouts are predictable for serialization and FFI.
* **RNA:** A reflection and access layer built on top of DNA. It allows the Qt6 editor to dynamically build property panels (e.g., `DragSpinBox` for transforms) and provides a bridge for future Python bindings.

### VPI (Vektor Platform Interface)
Instead of a generic library like GLFW, Vektor uses **VPI**—an internal abstraction layer that manages windows and graphics contexts. It handles native Qt6 window embedding and manages the lifecycle of OpenGL and Metal contexts across threads.

### Rust Compute Bridge
Performance-critical tasks are moved to the `/compute` workspace. We use `cxx` to bridge C++ and Rust with minimal overhead. 
* **SIMD:** Matrix and vector operations are offloaded to Rust's `wide` crate for hardware-level optimization.
* **Parallelism:** Heavy-duty calculations (like world-space matrix updates) are distributed via `rayon`.

### Dual-Backend HAL
The renderer is designed to be backend-agnostic. 
* On **macOS**, the engine defaults to **Metal** using `CAMetalLayer` and custom pipeline state objects.
* On **Linux/Windows**, it utilizes **OpenGL 4.1 Core Profile**.
* Unified shader source using `.slang` files that compile to GLSL or Metallib on the fly.

## Project Structure

* `source/runtime/`: The core engine. Contains the ECS kernel, DNA/RNA definitions, the `vmo` (Object Execution) logic, and the Draw Manager.
* `source/editor/`: The Qt6 application. Implements the Viewport, Outliner, and Properties panels.
* `compute/`: Rust crates for SIMD math acceleration and FFI bridging.
* `intern/`: Foundation modules.
    * `vpi/`: Windowing and context abstraction.
    * `clog/`: A structured logging system with file/console dual-output.
    * `gaurdalloc/`: Guarded memory allocator for tracking leaks and corruption.
* `extern/`: Dependency management via CMake `FetchContent`.

## Building

**Requirements:**
* CMake 3.20+
* Qt6 (Core, Widgets, Gui, OpenGLWidgets)
* Rust Toolchain (Cargo/rustc)
* C++20 compliant compiler (Clang 12+, GCC 11+, or MSVC 19.29+)

```bash
# Clone the repo and submodules
git clone --recursive https://github.com/your-repo/vektor.git
cd vektor

# Standard CMake build
mkdir build && cd build
cmake ..
cmake --build . --config Release
