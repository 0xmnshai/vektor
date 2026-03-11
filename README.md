# Vektor

A hybrid C++20/Rust high-performance game engine and Qt6-powered editor. 

## Stack & Technologies
* **Language:** C++20, Rust, Python
* **Graphics API:** OpenGL 4.1 / Apple Metal Cpp (Hardware Abstraction Layer)
* **Shader Compiler:** Slang (GLSL/SPIR-V cross-compilation)
* **Math Pipeline:** GLM (C++), Rayon (Rust multithreading), `std::simd`/`wide` AVX intrinsics (Rust SIMD)
* **ECS:** EnTT 
* **Editor UI:** Qt6 (QOpenGLWidget, Docking system)
* **Scripting:** Python 3 (pybind11 C++ bindings, PyO3 Rust bindings) (planned)
* **Asset Importers:** Assimp, stb_image (planned)
* **Audio:** OpenAL Soft (planned)
* **Physics:** Jolt / Box2D (planned)

## Architecture Overview

* **GHOST (Generic Handy Operating System Toolkit):** Abstract C++ platform layer for window, input, and context management. Backend implementations include standalone GLFW, Winit (Rust), and embedded Qt6.
* **DNA & RNA Data Model:** 
  * **DNA:** C-struct POD (Plain Old Data) layouts for game state. Ensure 100% flat memory representation for direct `.vmap` serialization.
  * **RNA:** Metadata reflection system pointing to DNA structs. Used by Qt6 to auto-generate `QDoubleSpinBox` / UI components, and by `pybind11` for script property exposure.
* **Kernel & ECS:** C++ EnTT registry wrapping DNA structs. Systems copy data to/from external physics/audio libraries (e.g., Jolt transforms synced back to `Transform` components).
* **Render Pipeline:** Forward-rendering pipeline. Rust Rayon calculates multithreaded Frustum Culling visibility arrays passed directly to the C++ Render Graph.

## Modules

* `compute/`: Rust workspace. Exposes math acceleration (`math_accel`) and pathfinding C-FFI layers via `cxx`. 
* `intern/`: Independent C++ foundation modules (GHOST, custom Guarded Memory Allocator, `spdlog` wrappers).
* `source/runtime/`: The core engine static library (`.a`/`.lib`). Contains the ECS kernel, renderer HAL, and resource managers.
* `source/editor/`: Qt6 executable. Links to `runtime`. Implements `ViewportWidget`, `AssetBrowser`, `Outliner`, and `InspectorPanel` via modular Space views.
* `assets/`: Default shaders (Slang), textures, and models.

## Build

Requires CMake 3.20+, Qt6, and a Rust toolchain (Cargo).

```bash
mkdir build && cd build
cmake ..
cmake --build .
```
