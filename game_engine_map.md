# Vektor Engine - Master Design Document

**Version:** 1.0.0
**Target:** Industry-Grade 3D Game Engine (Unreal/Unity rival)
**Standards:** C++20, Data-Oriented (ECS), Python Scripting, OpenGL 4.6 (Vulkan Ready)

---

## 1. Core Technology Stack

This selection balances the user's constraints (OpenGL, Box2D) with the goal of a "Professional 3D Engine".

| Module            | Library / Tech             | Description                                                            |
| :---------------- | :------------------------- | :--------------------------------------------------------------------- |
| **Core Language** | **C++20**                  | Modules, Concepts, Coroutines, Span.                                   |
| **Windowing**     | **GLFW**                   | Robust, multi-platform window creation and input handling.             |
| **Graphics API**  | **OpenGL 4.6**             | (User Request) Primary backend. Abstracted to allow Vulkan later.      |
| **Shading**       | **Slang**                  | Modern shading language. Compiles to SPIR-V/GLSL. Hot-reloadable.      |
| **ECS**           | **EnTT**                   | The industry-standard C++ ECS. Cache-local, zero-overhead.             |
| **Physics (3D)**  | **Jolt Physics**           | AAA-grade 3D physics (used in _Horizon Forbidden West_).               |
| **Physics (2D)**  | **Box2D**                  | (User Request) 2D physics for sprites/UI physics.                      |
| **Scripting**     | **Python (pybind11)**      | (User Request) Real-time scripting, simpler than Lua for complex data. |
| **UI (Tools)**    | **Dear ImGui**             | Editor interface. Docking, Nodes, Plotting enabled.                    |
| **Asset Import**  | **tinygltf, tinyobj, stb** | Fast, compliant loading for Models/Textures.                           |
| **Logging**       | **spdlog**                 | Async, colorized, structured logging.                                  |
| **Serialization** | **cereal** or **YAML-cpp** | Reflection-based asset saving/loading.                                 |
| **Math**          | **GLM**                    | OpenGL Mathematics (Standard in graphics dev).                         |

---

## 2. The "Vektor" Folder Structure

The Vektor project follows a structure inspired by Blender's architecture, separating the core kernel (`intern`), the application layer (`source`), and external dependencies (`extern`).

### 2.1. Root Directory (`vektor/`)

```text
vektor/
├── assets/                 # Game assets (Default models, textures, shaders)
├── extern/                 # Third-party libraries (Dependencies)
├── intern/                 # The Engine Core (Kernel Library) - Independent of App Logic
├── source/                 # Application Source (Editor, Runtime, Launcher)
├── scripts/                # Utility scripts (Python, Shell)
├── sdk/                    # Public API headers (for Game Logic / Plugins)
├── tools/                  # Build tools / Scripts
├── CMakeLists.txt          # Root Build Script
├── game_engine_map.md      # This Documentation
└── sample.vproj            # Sample Project File
```

### 2.2. The Engine Core (`vektor/intern/`)

This directory contains the low-level building blocks of the engine. These libraries are designed to be reusable and independent of the higher-level application logic.

#### `intern/atomic/` (Atomic Operations)
Platform-agnostic atomic operations and synchronization primitives for thread-safe programming.
- `atomic_ops.h`: Main interface for atomic operations.
- `atomic_ops_unix.h`: Unix-specific implementation.

#### `intern/clog/` (Logging System)
Custom logging system (wrapping `spdlog`) providing structured logging macros.
- `VLOG_log.h`: Defines logging macros like `VLOG_INFO`, `VLOG_ERROR`.
- `vlog.cc`: Implementation of the logger wrapper.

#### `intern/core/` (Core Utilities)
Fundamental core utilities, global definitions, and system-wide helpers.

#### `intern/cyrcles/` (Math Library)
Math and utility library (Cycles-inspired), containing vector math and common algorithms used throughout the engine.

#### `intern/ecs/` (Entity Component System)
The Entity Component System implementation. Handles entity storage, component pools, and system execution order.

#### `intern/gaurdalloc/` (Guarded Allocator)
Guarded Memory Allocator for debugging and tracking memory usage.
- Wraps `malloc` to add guard bytes (`0xDEADBEEF`) around allocations.
- Tracks memory usage stats and detects buffer overflows/leaks.
- `mallocn.c`: Main allocator implementation.
- `memory_usage.cc`: Statistics tracking.

#### `intern/platform/` (Platform Abstraction)
The "Ghost" platform abstraction layer. Handles window creation, input processing, and OS-specific features (Win32, Cocoa, X11).

#### `intern/plugins/` (Plugin System)
Plugin system infrastructure for loading dynamic libraries at runtime, allowing for modular extensions to the engine.

#### `intern/asset/` (Asset Management)
Core asset management and file format importers. Handles loading and processing of game assets.

### 2.3. Application Source (`vektor/source/`)

This directory contains the high-level application code, divided into the Creator (entry point), Editor, and Runtime.

#### `source/creator/` (The Launcher)
The entry point of the application.
- `creator.cc`: The `main()` function. Initializes the engine environment and launches the application.
- `creator_args.cc`: Command-line argument parsing and handling.
- `creator_global.cc`: Manages global application state.
- `creator_signals.cc`: Signal handling for crash reporting and graceful shutdowns.
- `build_info.c`: Contains build metadata.

#### `source/runtime/` (The Engine Runtime)
The core runtime engine that powers the game.
- `kernel/`: The main loop and system managers.
- `dna/`: List of Data Structure Definitions (SDNA). Defines the memory layout of assets for serialization.
- `vklib/`: Vulkan Graphics Library wrapper. Handles RHI (Render Hardware Interface) interactions.
- `utils/`: Runtime-specific utilities and helper functions.

#### `source/editor/` (The Editor)
The content creation suite (Editor) used to build games.
- `imgui/`: Integration of Dear ImGui for the editor UI.
- `inspectors/`: Logic for inspecting and editing components and assets in the editor.
- `viewport/`: 3D Viewport rendering and interaction logic.

### 2.4. External Dependencies (`vektor/extern/`)

Managed 3rd-party libraries. This folder contains external code that the engine links against, ensuring a self-contained build environment.


---

## 3. Architecture Details

### 3.1. Memory Layout (Blender-Like)

The user explicitly requested Blender's memory model. We will implement `Vektor::Memory`.

- **Guarded Allocator**:
  - Functions: `MemAlloc(size, desc)`, `MemFree(ptr)`.
  - **Debug**: Adds `0xDEADBEEF` padding. Tracks total bytes per "Description".
  - **Release**: Compiles down to `malloc` for speed.
- **Linear Allocator**: For per-frame rendering commands (cleared every frame).
- **Pool Allocator**: For ECS Components (EnTT does this internally, but we can supply our custom allocator to EnTT).

### 3.2. Rendering Pipeline (RHI)

To support the feature list (PBR, HDR, Shadows), we need a **Frame Graph** approach, not just "draw calls".

1.  **RenderPass**: A struct defining inputs (Textures) and outputs (Framebuffer).
2.  **Forward+ / Deferred**:
    - **G-Buffer Pass**: Render Albedo, Normal, Material data to textures (GL_RGBA16F).
    - **Lighting Pass**: Compute PBR (Cook-Torrance) lighting using G-Buffer.
    - **Post-Process**: Tone Mapping (ACES), Gamma Correction, FXAA.
3.  **Materials**:
    - Defined in **Slang**.
    - "Metallic-Roughness" workflow standard.
    - Engine hot-reloads `.slang` files and updates Pipelines automatically.

### 3.3. Entity-Component-System (ECS)

All game objects are entities.

- **Scene Graph**: Implemented as a `RelationshipComponent` (Parent/Children IDs) within EnTT.
- **Components**:
  - `Transform`: Position, Rotation, Scale.
  - `MeshRenderer`: Handle to Mesh + Material.
  - `Light`: Point/Spot/Directional data.
  - `RigidBody`: Jolt Physics ID.
  - `Script`: Python Class Instance.

### 3.4. Python Scripting

- **Binder**: `pybind11` exposes `Vektor::Entity`, `Vektor::Transform`, etc.
- **Workflow**:
  - User creates `PlayerController.py` inheriting from `vektor.Component`.
  - Engine instantiates this class on `OnStart()`.
  - Engine calls `update(dt)` on the Python instance every frame.

---

## 4. Implementation Guidelines (Coding Standards)

1.  **Prefixes**: Use namespaces, NOT prefixes.
    - BAD: `vk_render_pass`
    - GOOD: `Vektor::Render::Pass`
2.  **Files**:
    - Interfaces in `.h` (pure virtual if RHI).
    - Implementations in `.cpp`.
3.  **Initialization**:
    - No globals. Use a `ServiceLocator` or `Context` pattern passed to systems.
    - `Vektor::Engine::Get().GetRenderer()`

---

## 5. Blender-Specific Adoptions (The "Secret Sauce")

To truly match Blender's "Industry Grade" feel, we adopt these specific technologies and patterns found in the `blender/source` and `blender/extern` directories.

### 5.1. Naming Conventions (Strict)

We adopt Blender's prefix system to prevent symbol collisions and enforce architectural layering.

- **`BLI_` (Blender Library) -> `VKT_` (Vektor Core)**:
  - Pure C/C++ utilities with _zero_ engine dependencies.
  - Examples: `VKT_string`, `VKT_vector`, `VKT_path_util`.
- **`BKE_` (Blender Kernel) -> `ENG_` (Engine Kernel)**:
  - Core logic involving the Scene Graph or Database.
  - Examples: `ENG_scene_add_entity`, `ENG_mesh_calc_normals`.
- **`DNA_` (Data Structure) -> `Schema_`**:
  - POD (Plain Old Data) structs that define the file format.
  - Example: `struct Schema_Transform { float loc[3]; };`
- **`RNA_` (Reference) -> `Meta_`**:
  - Reflection definitions used by Python/UI.
- **`WM_` (Window Manager) -> `App_`**:
  - Windowing, Event Queue, and Main Loop logic.
- **`GPU_`**:
  - Hardware abstraction (GL/Vulkan).

### 5.2. Libraries from Blender's `extern/`

We integrate these specific libraries that Blender uses for robustness:

1.  **`audaspace`**: (Blender's Audio Engine). High-level audio graph (filters, 3D sound).
2.  **`mantaflow` / `bullet`**: (Physics). _Choice:_ We stuck with **Jolt** (Modern) over Bullet (Legacy), but use **Mantaflow** concepts for fluids if needed.
3.  **`lzma` / `zlib`**: For compressing `.vmap` project files.
4.  **`gflags` / `glog`**: Blender uses `glog`; we chose `spdlog` (faster), but the _concept_ of structured logging is the same.
5.  **`pcre`**: Regex support (if not using C++ std::regex).
6.  **`wcwidth`**: For correct text rendering in the Console.

### 5.3. The "Ghost" System (GHOST)

Blender's `intern/ghost` is its Platform Abstraction Layer.

- **Vektor Equivalent**: `Source/Runtime/Platform/Ghost`.
- **Responsibility**:
  - Handles `Win32`, `Cocoa`, `X11` internals hidden behind a `GHOST_System` interface.
  - Supports: Multi-window, Tablet pressure (Wintab), Copy/Paste.

---

## 6. Core Workflows (The "How-To")

### 5.1. Project & Scene Serialization (Saving/Loading)

We use **cereal** (or YAML-cpp) to serialize the **EnTT Registry**.

1.  **Project File (`.vproj`)**: A JSON file containing project settings (Name, Start Scene Path, Asset Directory).
2.  **Asset Registry**: A database (HashMap) identifying assets by **GUID**.
    - `Map<GUID, Path>`: `0x1A2B...` -> `Assets/Models/Hero.gltf`.
    - When saving a component, we save the GUID, not the path.
3.  **Scene Saving (`.vmap`)**:
    - **Snapshot**: `entt::snapshot` iterates all entities.
    - **Archive**: We serialize each component.
      ```cpp
      template<class Archive>
      void serialize(Archive & archive, TransformComponent & t) {
          archive(t.position, t.rotation, t.scale);
      }
      ```
4.  **Scene Loading**:
    - Clear current Registry.
    - Load `.vmap` JSON.
    - Reconstruct entities and components.
    - **Post-Load**: Re-link `ScriptComponent` GUIDs to actual Python instances.

### 5.2. Rendering Flow (Frame Graph)

Rendering is not just "loop and draw". It's a pipeline.

1.  **Culling**: `RenderSystem` iterates all `MeshRenderer` components. Checks AABB against the active Camera's Frustum. Visible entities are added to a `RenderQueue`.
2.  **Sorting**:
    - **Opaque**: Front-to-back (minimie overdraw).
    - **Transparent**: Back-to-front (render correctness).
3.  **Pass 1: Shadow Map**: Render visible objects to a Depth Texture from the Light's point of view.
4.  **Pass 2: Geometry (G-Buffer)**: Render Objects to Multiple Render Targets (Albedo, Normal, MetallicRoughness).
5.  **Pass 3: Lighting**: Draw a fullscreen quad. Shader calculates lighting using G-Buffer + ShadowMap + IBL.
6.  **Pass 4: UI/Editor**: Render ImGui overlay on top.

### 5.3. Script Execution Lifecycle

How Python drives the game.

1.  **Setup**:
    - User attaches `ScriptComponent` with module name `"PlayerController"`.
2.  **Instantiation (`OnStart`)**:
    - Engine looks up `"PlayerController"` in Python VM.
    - Creates instance: `py::object instance = pythonModule.attr("PlayerController")()`.
    - Injects C++ Entity ID into the Python object so it can call `self.entity.get_component<Transform>()`.
3.  **Loop (`OnUpdate`)**:
    - Engine iterates all `ScriptComponent`s.
    - Calls `instance.attr("on_update")(deltaTime)`.
4.  **Hot-Reload**:
    - If user saves `.py` file, we verify syntax.
    - We serialize the _data_ of the current instance (vars like `speed=50`).
    - Reload module. Re-instantiate class.
    - Apply saved data back to new instance.

### 5.4. Gameplay Loop (The `Application::Run` Loop)

1.  **Poll Events**: `glfwPollEvents()` -> Updates Input State.
2.  **Time**: Calculate `deltaTime`.
3.  **Script Update**: Run Python logic (moves transforms, spawns entities).
4.  **Physics Step**: `Jolt::PhysicsSystem::Update(deltaTime)`. Syncs Jolt Bodies back to EnTT Transforms.
5.  **Audio**: Update listener position.
6.  **Render**: Execute Render Pipeline (Camera sees the new positions).
7.  **Swap Buffers**: `glfwSwapBuffers()`.

---

## 7. Feature Implementation Roadmap

### Phase 1: Foundation

- [ ] Setup CMake with `External/` folder.
- [ ] Implement `Vektor::Core::Memory` (The Guarded Allocator).
- [ ] Initialize GLFW window and OpenGL 4.6 context.
- [ ] Integrate `spdlog` and `ImGui`.

### Phase 2: Rendering (The "Pretty" Stuff)

- [ ] **RHI**: Abstract Buffer/VertexArray.
- [ ] **PBR**: Implement BRDF shader in Slang.
- [ ] **Loading**: Use `tinygltf` to load a damaged helmet model.
- [ ] **HDR**: Render to Float16 buffer, apply Tone Mapping.

### Phase 3: The Engine Core

- [ ] **ECS**: Integrate EnTT. Create `Scene` class.
- [ ] **Scene Graph**: Implement parenting/transforms.
- [ ] **Physics**: Integrate Jolt (connect Transform -> Jolt Body).

### Phase 4: Editor Tools

- [ ] **Hierarchy Panel**: Loop EnTT registry, show entities.
- [ ] **Inspector**: Show components of selected entity (ImGui).
- [ ] **Gizmo**: Integreate `ImGuizmo` for editing transforms.
- [ ] **Console**: Redirect `spdlog` output to an ImGui window.

### Phase 5: Advanced Features

- [ ] **Shadows**: Cascaded Shadow Maps (CSM).
- [ ] **Scripting**: Initialize Python VM, bind Transform.
- [ ] **Post-Process**: Bloom, SSAO, FXAA.

---

## 8. Implementation Reference (The "Snippet" Guide)

Use these patterns to start coding. They fuse Blender's concepts with Modern C++.

### 7.1. Memory: The Guarded Allocator

Blender uses `MEM_mallocN`. We implement `Vektor::Memory` with checks.

**`Source/Runtime/Core/Memory/Allocator.h`**

```cpp
namespace Vektor::Core::Memory {
    struct MemHeader {
        size_t size;
        const char* desc;
        uint32_t guard; // 0xDEADBEEF
    };

    // Global tracker (Thread-safe)
    static std::atomic<size_t> g_TotalAllocated = 0;

    template<typename T, typename... Args>
    T* Alloc(const char* desc, Args&&... args) {
        size_t size = sizeof(T);
        // Allocate Size + Header + Footer
        void* raw = malloc(sizeof(MemHeader) + size + sizeof(uint32_t));

        MemHeader* head = (MemHeader*)raw;
        head->size = size;
        head->desc = desc;
        head->guard = 0xDEADBEEF;

        void* data = (char*)raw + sizeof(MemHeader);

        // Placement New
        return new(data) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void Free(T* ptr) {
        if(!ptr) return;
        ptr->~T(); // Destruct

        char* raw = (char*)ptr - sizeof(MemHeader);
        MemHeader* head = (MemHeader*)raw;

        // CHECK GUARD
        if(head->guard != 0xDEADBEEF) {
            SPDLOG_CRITICAL("Memory Corruption detected at {}", (void*)ptr);
            abort();
        }

        free(raw);
    }
}
```

### 7.2. Project Saving (Serialization)

Using **cereal** to save the EnTT Registry to `.vmap`.

**`Source/Runtime/Resource/Serializer.cpp`**

```cpp
#include <cereal/archives/json.hpp>
#include <entt/entt.hpp>

void SaveScene(entt::registry& registry, const std::string& path) {
    std::ofstream os(path);
    cereal::JSONOutputArchive archive(os);

    // Snapshot: Serialize all entities with specific components
    entt::snapshot{registry}
        .entities(archive)
        .component<TransformComponent>(archive)
        .component<MeshComponent>(archive)
        .component<ScriptComponent>(archive);
}

// TransformComponent serialization support
template <class Archive>
void serialize(Archive& ar, TransformComponent& t) {
    ar(CEREAL_NVP(t.position), CEREAL_NVP(t.rotation), CEREAL_NVP(t.scale));
}
```

### 7.3. Events: The Bus

A modern Observer pattern for decouling Logic from Input.

**`Source/Runtime/Core/Events/EventBus.h`**

```cpp
namespace Vektor::Core {
    struct Event { virtual ~Event() = default; };
    struct KeyPressedEvent : Event { int key; };

    class EventBus {
    public:
        using Handler = std::function<void(Event&)>;

        void Subscribe(std::type_index type, Handler handler) {
            subscribers_[type].push_back(handler);
        }

        template<typename T>
        void Publish(T& event) {
            for(auto& handler : subscribers_[typeid(T)]) {
                handler(event);
            }
        }
    private:
        std::map<std::type_index, std::vector<Handler>> subscribers_;
    };
}
```

### 7.4. Scripting: Python Component

Embedding Python to control an Entity.

**`Source/Runtime/Function/Scripting/ScriptEngine.cpp`**

```cpp
#include <pybind11/pybind11.h>
namespace py = pybind11;

struct ScriptComponent {
    std::string moduleName;
    py::object instance;
};

void ScriptSystem::OnStart(entt::registry& reg) {
    auto view = reg.view<ScriptComponent>();
    for(auto entity : view) {
        auto& script = view.get<ScriptComponent>(entity);

        // 1. Import Module
        py::object module = py::module::import(script.moduleName.c_str());

        // 2. Instantiate Class (assumes Class name == Module name)
        py::object classObj = module.attr(script.moduleName.c_str());
        script.instance = classObj();

        // 3. Inject Entity ID
        script.instance.attr("entity_id") = (uint32_t)entity;

        // 4. Call OnStart
        if(py::hasattr(script.instance, "on_start"))
            script.instance.attr("on_start")();
    }
}
```

### 7.5. Asset Loading: Mesh

Loading a GLTF model into a `Mesh` component.

**`Source/Runtime/Resource/Importer/ModelImporter.cpp`**

```cpp
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

Handle<Mesh> LoadMesh(const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadASCIIFromFile(&model, &err, &warn, path)) {
        SPDLOG_ERROR("Failed to load GLTF: {}", err);
        return {};
    }

    // Convert GLTF primitives to Vektor buffers
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // ... (Iterate accessors and fill vectors) ...

    // Upload to GPU (OpenGL)
    uint32_t vao, vbo, ebo;
    glCreateVertexArrays(1, &vao);
    glNamedBufferStorage(vbo, vertices.size() * sizeof(Vertex), vertices.data(), 0);

    return Render::AssetManager::Get().CreateMesh(vao, indices.size());
}
```

### 7.6. Object Properties (Reflection)

Blender uses RNA. We use an `Inspector` system with ImGui.

**`Source/Editor/Panels/Inspector.cpp`**

```cpp
void DrawInspector(entt::registry& reg, entt::entity e) {
    if(reg.all_of<TransformComponent>(e)) {
        auto& t = reg.get<TransformComponent>(e);
        if(ImGui::CollapsingHeader("Transform")) {
            ImGui::DragFloat3("Position", &t.position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &t.rotation.x, 1.0f);
            ImGui::DragFloat3("Scale", &t.scale.x, 0.1f);
        }
    }

    if(reg.all_of<LightComponent>(e)) {
        auto& l = reg.get<LightComponent>(e);
        ImGui::ColorEdit3("Color", &l.color.r);
        ImGui::DragFloat("Intensity", &l.intensity);
    }
}
```

### 7.7. Texture Loading (stb_image)

Loading a texture to OpenGL.

**`Source/Runtime/Resource/Importer/TextureImporter.cpp`**

```cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Handle<Texture> LoadTexture(const std::string& path) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if(!data) {
        SPDLOG_ERROR("Failed to load image: {}", path);
        return {};
    }

    uint32_t rendererID;
    glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glTextureStorage2D(rendererID, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(rendererID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return Render::AssetManager::Get().CreateTexture(rendererID, width, height);
}
```

### 7.8. Shader Compilation (Slang)

Compiling `.slang` to SPIR-V/GLSL at runtime.

**`Source/Runtime/Render/Shader/ShaderCompiler.cpp`**

```cpp
// Pseudocode for Slang Integration
slang::IGlobalSession* g_SlangSession;

std::vector<uint32_t> CompileSlangToSPIRV(const std::string& path) {
    slang::SessionDesc sessionDesc = {};
    sessionDesc.targets = &targetDesc; // Vulkan/SPIR-V

    ComPtr<slang::ISession> session;
    g_SlangSession->createSession(sessionDesc, session.writeRef());

    ComPtr<slang::IModule> module;
    session->loadModule(path.c_str(), module.writeRef());

    ComPtr<slang::IEntryPoint> entryPoint;
    module->findEntryPointByName("main", entryPoint.writeRef());

    // Compile and get Kernel Code
    ComPtr<slang::IBlob> spirvCode;
    entryPoint->getKernelCode(spirvCode.writeRef());

    return { (uint32_t*)spirvCode->getBufferPointer(), ... };
}
```

---

## 8. The Python-Driven UI System (Imitating Blender)

You requested the "Blender workflow": define UI in Python, render in C++. Here is the architecture.

### 8.1. The Plan

1.  **C++**: Expose `ImGui` functions to Python via `pybind11`.
2.  **Python**: Create a `Panel` base class. Users inherit this to define layout.
3.  **Bridge**: The C++ Editor loop iterates registered Python Panels and calls `.draw()`.

### 8.2. C++ Binding (`Source/Runtime/Function/Scripting/UIBinder.cpp`)

We wrap ImGui calls into a module `vektor.ui`.

```cpp
#include <pybind11/pybind11.h>
#include <imgui.h>

void BindUI(py::module& m) {
    auto ui = m.def_submodule("ui");

    ui.def("button", [](const char* label) {
        return ImGui::Button(label);
    });

    ui.def("label", [](const char* text) {
        ImGui::Text("%s", text);
    });

    ui.def("slider_float", [](const char* label, float& v, float min, float max) {
        return ImGui::SliderFloat(label, &v, min, max);
    });

    // The Layout Context passed to Python
    py::class_<UILayout>(ui, "Layout")
        .def("row", &UILayout::Row)
        .def("prop", &UILayout::Prop);
}
```

### 8.3. Python Framework (`Assets/Scripts/vektor_ui.py`)

This mimics Blender's `bpy.types.Panel`.

```python
import vektor.ui as ui

class Panel:
    bl_label = "Undefined"
    bl_region_type = "UI"

    def draw(self, context):
        pass

# Registry to hold all panels
_registered_panels = []
def register_panel(cls):
    _registered_panels.append(cls())

def draw_region(region_type):
    for panel in _registered_panels:
        if panel.bl_region_type == region_type:
            if ui.begin_panel(panel.bl_label):
                panel.draw(context=None)
                ui.end_panel()
```

### 8.4. User Code (Example)

The user writes this to make a new Tool Panel.

```python
import vektor.ui as ui
from vektor_ui import Panel, register_panel

class OBJECT_PT_Transform(Panel):
    bl_label = "Transform"
    bl_region_type = "WINDOW"

    def draw(self, context):
        obj = context.active_object
        ui.label("Location")
        ui.drag_float3(obj.location)

        if ui.button("Randomize"):
            obj.randomize_transform()

register_panel(OBJECT_PT_Transform)
```

### 8.5. The Render Loop (C++)

The C++ application drives the Python drawing.

```cpp
void EditorLayer::OnImGuiRender() {
    // 1. Setup Context
    py::object ui_module = py::module::import("vektor_ui");

    // 2. Draw Side Region
    ImGui::Begin("Properties");
    ui_module.attr("draw_region")("WINDOW");
    ImGui::End();
}
```

---

## 9. The DNA / RNA System (Data & Reflection)

You asked: _"What is this RNA-DNA thing?"_

### 9.1. The Concept (Blender's Secret Weapon)

Blender separates **Data Storage** from **Data Access**.

1.  **DNA (Data Name)**: The **Storage Layer**.
    - **What it is**: Pure C structs (PODs) defined in `.h` files.
    - **Crucial Feature**: Blender's build system (`makesdna`) parses these headers and generates a "DNA" signature. This allows Blender to load `.blend` files from 10 years ago—it knows exactly how the memory layout changed.
    - **In Vektor**: We call this **Schemas**.

2.  **RNA (RoNA / Reflection)**: The **Access Layer**.
    - **What it is**: An API that wraps DNA.
    - **Why?**: DNA is just raw bytes. RNA tells Python/UI: _"This struct has a property named 'location', it is a float array[3], and min/max is -1000 to 1000"_.
    - **In Vektor**: We call this **Reflection** (built on `entt::meta` or `rttr`).

### 9.2. Implementing "DNA" (The Schema)

In Vektor, our "DNA" is the ECS Component structs. They must be "Trivial" (no pointers, no logic) so we can save them directly to disk.

**`Source/Runtime/Function/ECS/Schema/Schema_Transform.h`**

```cpp
namespace Vektor::Schema {
    // DNA: Pure Data. No Functions.
    struct Transform {
        float location[3];
        float rotation[4]; // Quaternion
        float scale[3];

        // Metadata for the "DNA Parser" (Serialization)
        template<class Archive>
        void serialize(Archive & archive) {
            archive(location, rotation, scale);
        }
    };
}
```

### 9.3. Implementing "RNA" (The Reflection)

We need a way for Python and the UI to discover these fields _without_ hardcoding every button. We use `entt::meta`.

**`Source/Runtime/Core/Reflection/MetaRegistry.cpp`**

```cpp
void RegisterMetaTypes() {
    using namespace entt::literals;

    entt::meta<Schema::Transform>()
        .type("Transform"_hs) // The "Name"
        .data<&Schema::Transform::location>("location"_hs) // The Property
        .prop("tooltip"_hs, "The 3D position of the object");
}
```

### 9.4. The Usage (How UI uses RNA)

Now, our Generic Inspector (Section 7.6) becomes truly powerful. It doesn't need to know about `Transform` component specifically. It iterates the _Meta_ properties.

**`Source/Editor/Panels/GenericInspector.cpp`**

```cpp
void DrawGenericInspector(entt::registry& reg, entt::entity e) {
    // Loop through all components on this entity
    for(auto [id, storage] : reg.storage()) {
        if(storage.contains(e)) {
            // Get the Reflection Type (RNA)
            auto type = entt::resolve(id);

            if(ImGui::CollapsingHeader(type.info().name().data())) {
                void* instance = storage.value(e);

                // Iterate Properties (location, rotation...)
                for(auto data : type.data()) {
                    auto name = data.prop("name"_hs).value().cast<const char*>();

                    // Automatically draw the right widget
                    if(data.type().id() == entt::type_id<float[3]>()) {
                        // Assuming 'instance' is correctly cast or handled by meta
                        // For demonstration:
                        // float* val = ... get pointer from meta ...
                         ImGui::Text("Field: %s", name);
                    }
                }
            }
        }
    }
}
```

---

## 10. The Engine Kernel (`ENG` / `blenkernel`)

You asked to implement **BlenKernel (BKE)**. In Vektor, we call this the **Engine Kernel (`ENG`)**.

### 10.1. What is the Kernel?

- **DNA** = State (Structs).
- **Kernel** = Logic (Functions).
- **UI** = Interaction.

The Kernel is the library of high-level functions that manipulate the Data. **Crucially**, it depends on _nothing_ from the UI. You should be able to run Kernel functions in a headless command-line tool.

### 10.2. Naming Convention

As defined in Section 5, we use the **`ENG_`** prefix.

### 10.3. Kernel Examples (Snippets)

**1. Scene Manipulation (`Source/Runtime/Kernel/ENG_scene.cpp`)**
Logic to add an object. It doesn't "draw" anything; it just updates the Data (ECS).

```cpp
// BKE_scene_add_object -> ENG_scene_spawn_entity
entt::entity ENG_scene_spawn_entity(entt::registry& reg, const char* name, Vektor::Math::vec3 loc) {
    // 1. Create the ID
    auto entity = reg.create();

    // 2. Add DNA Data
    reg.emplace<Schema::Transform>(entity, loc, Vektor::Math::quat::identity(), Vektor::Math::vec3::one());
    reg.emplace<Schema::Name>(entity, name);

    // 3. Mark as "Dirty" for the Depsgraph
    ENG_depsgraph_tag(entity);

    return entity;
}
```

**2. Mesh Algorithms (`Source/Runtime/Kernel/ENG_mesh.cpp`)**
Calculating normals—pure math on Data.

```cpp
// BKE_mesh_calc_normals -> ENG_mesh_calc_normals
void ENG_mesh_calc_normals(Schema::Mesh& mesh) {
    // Access raw DNA arrays
    auto& verts = mesh.vertices; // std::vector<vec3>
    auto& indices = mesh.indices;

    // Reset normals
    std::fill(mesh.normals.begin(), mesh.normals.end(), vec3(0));

    // Iterate Triangles (Topology)
    for(size_t i = 0; i < indices.size(); i += 3) {
        vec3 v0 = verts[indices[i]];
        vec3 v1 = verts[indices[i+1]];
        vec3 v2 = verts[indices[i+2]];

        vec3 normal = Cross(v1 - v0, v2 - v0);

        // Accumulate (Smooth Shading logic)
        mesh.normals[indices[i]] += normal;
        mesh.normals[indices[i+1]] += normal;
        mesh.normals[indices[i+2]] += normal;
    }

    // Normalize
    for(auto& n : mesh.normals) n = Normalize(n);
}
```

**3. Modifier Evaluation (`Source/Runtime/Kernel/ENG_modifier.cpp`)**
The "Stack" concept. `Array Modifier` example.

```cpp
void ENG_modifier_apply_array(Schema::Mesh& output, const Schema::ModifierArray& params) {
    size_t original_count = output.vertices.size();

    for(int i = 1; i < params.count; ++i) {
        vec3 offset = params.relative_offset * (float)i;

        // Duplicate Geometry
        for(size_t v = 0; v < original_count; ++v) {
            output.vertices.push_back(output.vertices[v] + offset);
            // ... copy indices with offset ...
        }
    }
}
```

---

## 11. Window Manager & Editors (`App` & `ED`)

You requested **Window Manager (WM)** and **Editors (ED)** layers.

### 11.1. Window Manager (`App_` / `WM_`)

The **Window Manager** connects the Operating System (Ghost) to the Blender Internal logic. It manages **Windows**, **Screens**, **Areas**, and the **Event Loop**.

- **Responsibility**: Open windows, poll events, dispatch events to the active Area (Editor).
- **Prefix**: **`App_`** (Vektor's equivalent of `WM_`).

**Snippet: The Main Event Loop (`Source/Runtime/Platform/App_manager.cpp`)**

```cpp
void App_main_loop() {
    while (App_window_is_open()) {
        // 1. Poll OS Events (Ghost Layer)
        GHOST_Event* evt = GHOST_PollEvent();

        // 2. Convert to Vektor Event
        AppEvent v_evt = App_event_convert(evt);

        // 3. Dispatch to Active Editor (e.g., 3D View or Script Editor)
        ED_area_handle_event(g_Context.active_area, v_evt);

        // 4. Update Engine Kernel (Physics, Scripts)
        ENG_update(g_Context.delta_time);

        // 5. Draw
        App_draw_update();
    }
}
```

### 11.2. Editors (`ED_`)

An **Editor** (Space) is a specific tool: 3D Viewport, Outliner, Graph Editor.

- **Responsibility**: Handle specific interactions (Gizmos in 3D View, Tree expanding in Outliner).
- **Prefix**: **`ED_`**.

**Snippet 1: Editor Definition (`Source/Editor/Spaces/ED_view3d.cpp`)**

```cpp
// Initialize the 3D Viewport Editor
void ED_view3d_init(SpaceLink* space) {
    SpaceView3D* s3d = (SpaceView3D*)space;
    s3d->camera_fov = 45.0f;
    s3d->render_mode = R_MODE_LIT;

    // Register generic Operators specific to this view
    ED_operator_add("VIEW3D_OT_rotate", ED_view3d_rotate_exec);
}

// The Draw Callback (called by App_draw_update)
void ED_view3d_draw(const bContext* C) {
    // 1. Setup Camera from View3D data
    Render::Camera cam = ED_view3d_camera_eval(C);

    // 2. Pass to Renderer
    Render::BeginScene(cam);
    Render::DrawScene(ENG_get_active_scene());

    // 3. Draw Gizmos (Overlay)
    if (ED_ctx_is_active(C)) {
        ED_gizmo_draw_transform();
    }

    Render::EndScene();
}
```

**Snippet 2: The Outliner Logic (`Source/Editor/Spaces/ED_outliner.cpp`)**

```cpp
void ED_outliner_build_tree(SpaceOutliner* space) {
    space->tree.clear();

    // Use Kernel to query Scene
    auto& registry = ENG_get_registry();

    for(auto entity : registry.view<Schema::Name>()) {
        TreeElement elem;
        elem.name = registry.get<Schema::Name>(entity).value;
        elem.icon = ICON_OBJECT_CUBE;
        elem.entity_id = entity;

        state->tree.push_back(elem);
    }
}
```

---

## 12. Coding Styles & Patterns (The "Vektor Style")

You requested the **EXACT** Blender style adapted for Vektor. We follow Blender's conventions (`source/blender/doc/guides/coding_style.md`) but mapped to our prefixes.

### 12.1. Prefix Naming Convention

Function names indicate their module layer. This prevents dependency hell.

| Blender Prefix | Vektor Prefix | Module              | Description                                         |
| :------------- | :------------ | :------------------ | :-------------------------------------------------- |
| **`BKE_`**     | **`ENG_`**    | **Engine Kernel**   | Core Logic. No UI deps. `ENG_scene_spawn(...)`      |
| **`WM_`**      | **`App_`**    | **Window Manager**  | Platform, Events, Main Loop. `App_window_open(...)` |
| **`ED_`**      | **`ED_`**     | **Editors**         | Tool/Space specifics. `ED_view3d_init(...)`         |
| **`UI_`**      | **`UI_`**     | **Interface**       | Widgets/Layouts. `UI_button_add(...)`               |
| **`DNA_`**     | **`Schema_`** | **Data Schema**     | Serialized Data (PODs). `Schema_Transform`          |
| **`RNA_`**     | **`Meta_`**   | **Reflection**      | Meta-data for UI/Scripts. `Meta_resolve(...)`       |
| **`BLI_`**     | **`VKT_`**    | **Vektor Lib**      | Generic Utilities. `VKT_string_utils`, `VKT_math`   |
| **`GPU_`**     | **`RHI_`**    | **Render Hardware** | OpenGL/Vulkan Abstraction. `RHI_texture_create`     |

### 12.2. Struct-Based OOP (The "Data" Layer)

For **Schemas** (Components), we use pure C-style structs to mimic Blender's DNA.

- **Inheritance**: We don't use C++ inheritance for Data. We use Composition.
- **Casting**: `(Schema_Transform*)ptr`.

```cpp
// DNA_object_types.h -> Schema_Object.h
struct Schema_Object {
    Schema_ID id; // First member "Inheritance"
    float loc[3];
    float rot[3];
    float scale[3];
};
```

### 12.3. Modern C++ (The "Logic" Layer)

For `ENG_`, `ED_`, and `App_`, we use **C++20**.

- **Namespace**: `Vektor::Kernel`, `Vektor::Editor`.
- **Files**: `.cpp` (Implementations), `.h` (Headers). API headers (`ENG_*.h`) are C-linkage friendly where possible.

### 12.4. Memory Management (`MEM_` -> `Vektor::Memory`)

**NEVER** use `malloc`/`free` or raw `new`/`delete`.
**ALWAYS** use our Guarded Allocator.

```cpp
// MEM_mallocN -> Vektor::Memory::Alloc
auto* ptr = Vektor::Memory::Alloc<MyClass>("MyDescriptiveTag");

// MEM_freeN -> Vektor::Memory::Free
Vektor::Memory::Free(ptr);
```

- **Why?**: Tracks usage stats per tag ("Mesh", "Texture", "Audio") and detects leaks on shutdown.

### 12.5. Lists (`ListBase` -> `std::vector` / `std::span`)

Blender uses intrusive linked lists (`ListBase`).
**Vektor** uses contiguous memory for cache locality (Data-Oriented).

- **Storage**: `std::vector<T>` inside the Registry.
- **Passing**: `std::span<T>` (Ptr + Size) to functions.

### 12.6. Context (`bContext` -> `Vektor::Context`)

Passed globally or per-function to answer "Where am I?".

```cpp
// bContext *C
struct Context {
    entt::registry* registry;
    entt::entity active_entity;
    void* active_editor; // (void*) to decouble
};

// CTX_data_active_object(C) -> Context::GetActiveEntity(C)
```

### 12.7. File & Variable Naming

- **Files**: `PascalCase` implies a Class/Module file. `snake_case` implies a Library/Utility file.
- **Variables**: `snake_case` (`my_variable`).
- **Members**: `snake_case_` (`variable_`).
- **Globals**: `g_PascalCase` (`g_Context`).
- **Constants**: `UPPER_CASE` (`MAX_ENTITIES`).

### 12.8. Defines & Macros

- **Prefix**: `VKT_`
- **Assert**: `VKT_ASSERT(condition, msg)`

```cpp
#ifdef VKT_DEBUG
    #define VKT_ASSERT(x) ...
#endif
```

---

## 13. External Libraries & Dependencies

You asked for a all libraries from Blender's `extern` folder (as seen in your screenshot) and their specific use cases in Vektor.

### 13.1. Core Engine Stack (The "Big Ones")

| Library    | Vektor Usage      | Blender Equivalent | Why?                                    |
| :--------- | :---------------- | :----------------- | :-------------------------------------- |
| **EnTT**   | **ECS & Signals** | `DNA` + `Main`     | Cache-local, zero-overhead ECS.         |
| **Jolt**   | **Physics (3D)**  | `bullet2`          | Faster, multithreaded, AAA-grade.       |
| **Box2D**  | **Physics (2D)**  | N/A                | For 2D games/UI physics (User Request). |
| **Slang**  | **Shaders**       | `glew` + GLSL      | Modern modular shading language.        |
| **ImGui**  | **UI**            | `interface`        | Rapid tool development.                 |
| **Spdlog** | **Logging**       | `glog`             | Fast async logging.                     |

### 13.2. Asset Management & Compression

#### **`tinygltf`** (Model Loading)

- **Role**: Loading `.gltf` / `.glb` files (Meshes, Materials, Animations).
- **Snippet**: See Section 7.5.

#### **`draco`** (Mesh Compression)

- **Role**: Decompressing Draco-encoded geometry within GLTF files.
- **Usage**: `tinygltf` links against this.
- **Context**: Essential for web-optimized assets or huge scenes.

#### **`ufbx`** (FBX Loading)

- **Role**: Loading `.fbx` files (Legacy support for Max/Maya pipelines).
- **Why**: Lightweight, single-header, safe FBX parsing (unlike the official SDK).
- **Integration**:

```cpp
#include <ufbx.h>
void LoadFBX(const char* path) {
    ufbx_load_opts opts = {0}; // Defaults
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(path, &opts, &error);
    if(!scene) SPDLOG_ERROR("Failed to load FBX: {}", error.description.data);
}
```

#### **`nanosvg`** (Vector Graphics)

- **Role**: Rendering SVG icons for the UI.
- **Usage**: Rasterize `.svg` icons to Texture Atlases at runtime for resolution-independent UI.

#### **`fast_float`** (Parsing)

- **Role**: Extremely fast string-to-float conversion.
- **Usage**: Used inside `.obj` or custom text parsers. 10x faster than `std::stof`.

### 13.3. Simulation & Mathematics

#### **`mantaflow`** (Fluids - Future)

- **Role**: Gas/Fluid simulation.
- **Plan**: Phase 5 (Advanced). Keep as a reference for now.

#### **`quadriflow`** (Remeshing)

- **Role**: Auto-retopology (Quad generation).
- **Use Case**: Editor tool for "fixing" scanned assets.

#### **`curve_fit_nd`** (Animation)

- **Role**: Curve fitting algorithms.
- **Use Case**: Compressing animation keyframes (simplifying generic curves).

#### **`rangetree`** (Space Partitioning)

- **Role**: Fast spatial queries (k-d tree / range tree alternative).
- **Use Case**: Accelerating "Select Objects in Box" or particle neighbor search.

#### **`xxhash`** (Hashing)

- **Role**: Fast non-cryptographic hash.
- **Usage**: Hashing string paths to `Guid` or `AssetID`.

```cpp
#define XXH_INLINE_ALL
#include <xxhash.h>
uint64_t GetAssetID(const std::string& path) {
    return XXH64(path.data(), path.size(), 0);
}
```

### 13.4. System & Platform

#### **`gflags`** (Command Line)

- **Role**: Parsing command line arguments.
- **Usage**: `./VektorEditor --project "C:/MyGame" --headless`.
- **Blender**: Heavily used for CLI args.

#### **`glog`** (Logging - Standard)

- **Note**: We chose **spdlog** as it is faster and header-only friendly, but `glog` is the Google standard used by Blender.

#### **`wcwidth`** (Text Layout)

- **Role**: Determining column width of Unicode characters.
- **Use Case**: Essential for the **Console Panel** to align text correctly (especially Emojis or CJK labels).

#### **`binreloc`** (Linux Paths)

- **Role**: Finding relative paths on Linux systems.
- **Use Case**: Making the Engine portable (run from USB) on Linux.

#### **`wintab`** (Input)

- **Role**: Tablet Pressure support on Windows.
- **Use Case**: Sculpting mode or Texture Painting in Vektor Editor.

#### **`xdnd`** (Linux Drag & Drop)

- **Role**: Drag and drop protocol for X11.
- **Use Case**: Dragging assets from OS file manager into Vektor Editor on Linux.

### 13.5. Testing

#### **`gtest` / `gmock`** (Unit Testing)

- **Role**: Google Test framework.
- **Usage**: Writing tests for the Kernel (`ENG_`).

```cpp
TEST(Kernel, SceneSpawn) {
    auto reg = Vektor::Context::CreateRegistry();
    auto e = ENG_scene_spawn_entity(reg, "Hero", {0,0,0});
    ASSERT_TRUE(reg.valid(e));
}
```

### 13.6. Graphics & Compute Utilites

#### **`vulkan_memory_allocator` (VMA)**

- **Role**: Managing GPU memory for Vulkan.
- **Status**: **Essential** once we move to Phase 2/3 (RHI).

#### **`renderdoc`** (Debugging)

- **Role**: API Capture integration.
- **Usage**: Triggering a frame capture from the Editor UI button.

```cpp
#include <renderdoc_app.h>
// ... Load lib ...
rdoc_api->TriggerCapture();
```

#### **`cuew` / `hipew`** (Compute)

- **Role**: Dynamic loading of CUDA / HIP libraries.
- **Use Case**: GPU-accelerated path tracing (Cycles-style) or physics (if we strictly needed CUDA).
- **Vektor Plan**: We prioritize **Compute Shaders (Slang)** over proprietary CUDA for game engine portability.

---

## 14. Implementation Roadmap (Zero to Hero)

You asked for a step-by-step guide to building Vektor from an empty folder to a working engine, following this exact architecture.

### 14.1. Phase 1: The Foundation (Build System & Platform)

**Goal**: Get a window opening and the project structure set up.

1.  **Repository Setup**:
    - Create `vektor/` root.
    - Create folders: `extern`, `intern`, `sdk`, `docs`, `tools`, `editor`, `runtime`.
    - Initialize `git`.
    - Create `.gitignore` (ignore `build/`, `.ds_store`, etc.).

2.  **Dependency Management**:
    - Populate `extern/`:
      - `git clone` standard libs: `glfw`, `imgui`, `entt`, `spdlog`.
      - Create `extern/CMakeLists.txt` to expose them as targets.

3.  **Build System (`CMake`)**:
    - Write root `CMakeLists.txt`.
    - Setup `tools/cmake/` modules (Macros for `vektor_add_library`).
    - Configure output directories (`bin/`, `lib/`).

4.  **Platform Layer (`intern/platform`)**:
    - Implement `intern/platform/window.h`.
    - Implement `intern/platform/ghost_glfw.cpp`.
    - **Milestone**: Run `VektorPlayer` and see a blank black window.

### 14.2. Phase 2: The Kernel (Memory & Math)

**Goal**: Robust low-level systems before higher logic.

1.  **Logging**:
    - Implement `intern/clog` (Generic wrapper around `spdlog`).
    - Define macros: `VLOG_INFO`, `VLOG_ERROR`.

2.  **Memory**:
    - Implement `intern/core/memory.h` (Guarded Allocator).
    - Overload `new`/`delete` for debug builds.
    - Verify leak detection works.

3.  **Math**:
    - Create `sdk/vektor_math.h`.
    - Typedef `glm` types (`vec3`, `mat4`, `quat`).

### 14.3. Phase 3: The Engine Core (ECS)

**Goal**: Data management and entity spawning.

1.  **Registry**:
    - Implement `intern/ecs/world.h` (Wrapper around `entt::registry`).
    - Hide EnTT headers from the public SDK (PIMPL or forward decls if possible, though EnTT is header-only).

2.  **Components (`intern/asset/types`)**:
    - Define `Schema_Transform`, `Schema_Name`, `Schema_Mesh`.
    - Write strict POD structs.

3.  **Kernel Logic (`intern/core`)**:
    - Implement `ENG_scene_spawn_entity(...)`.
    - **Milestone**: Create an entity, set its name, and print it to the log.

### 14.4. Phase 4: The Renderer (RHI & Triangle)

**Goal**: Drawing something to the screen.

1.  **RHI (`intern/gpu`)**:
    - Define `gpu_texture.h`, `gpu_buffer.h` interfaces.
    - Implement `intern/gpu/device/opengl` (easiest for start).

2.  **Shader Compiler**:
    - Integrate `Slang`.
    - Write a basic `triangle.slang`.

3.  **Render Loop**:
    - Connect `intern/renderer` to `intern/platform`.
    - **Milestone**: A colorful triangle on the screen.

### 14.5. Phase 5: The Editor (ImGui)

**Goal**: Interactive tools.

1.  **Imgui Integration**:
    - Initialize `ImGui` in `editor/main.cpp`.
    - Hook up Input events from `intern/platform`.

2.  **Viewport Panel**:
    - Render the Engine's TextureID to an ImGui Image.

3.  **Outliner**:
    - Iterate `intern/ecs` registry and list Entity Names.

4.  **Property Inspector**:
    - Reflection system (Blender's RNA equivalent).
    - Edit `Schema_Transform` values via UI.

### 14.6. Phase 6: Assets & Physics

**Goal**: A real 3D Scene.

1.  **Asset Pipeline**:
    - Implement `tinygltf` in `intern/asset/importer`.
    - Load a `.glb` mesh.

2.  **Physics**:
    - Integrate `Jolt`.
    - Sync `Rigidbody` component -> `Jolt Body` -> `Transform` component.

### 14.7. Phase 7: Scripting (Python)

**Goal**: User logic.

1.  **Python Embed**:
    - Link `python3`.
    - Expose `vektor` module (`pybind11`).

2.  **Script Component**:
    - `Schema_Script` stores module name.
    - `ENG_script_update` calls `update()` on python objects.

---

**Next Steps**:
Currently, we are at **Phase 1, Step 1**: "Repository Setup".
I am ready to run the commands to create this directory structure for you.

---

## 15. Hydra Integration (High-Performance Data Analysis)

Integration of the Hydra framework for advanced mathematical processing, massively parallel data analysis, and high-precision random number generation.

### 15.1. Location in Source Tree

We treat Hydra as a core mathematical dependency, placed in `extern/`.

```text
vektor/extern/hydra/
├── include/     # Hydra headers (functors, histograms, random)
└── src/
```

### 15.2. Usage Strategies (Where and How)

Hydra is not just a math library; it is a framework for **Massively Parallel Data Analysis**. We leverage it in three key areas of Vektor.

#### 1. High-Precision Randomness & PCG (`intern/pcg`)

Standard `rand()` is insufficient for procedural generation. Hydra offers **Random Number Generation** and **PDF Sampling**.

- **Use Case**: Spawning vegetation or loot based on complex Probability Density Functions (PDFs) rather than simple uniform distributions.
- **Implementation**:
  ```cpp
  #include <Hydra/Random.h>
  // Generate a particle with a specific energy distribution
  double energy = hydra::random::sample_pdf(my_custom_pdf);
  ```

#### 2. Statistical Profiling (`intern/profiler`)

Instead of simple "Average FPS", we use Hydra's **Histograms** to visualize performance distribution.

- **Use Case**: Creating a real-time histogram of **Frame Times** or **Memory Allocation Sizes** in the Editor Console.
- **Benefit**: See "spikes" and "stutters" that averages hide.
- **Implementation**:
  ```cpp
  // Capture frame time into a 1D Histogram
  static hydra::Histogram1D frame_time_hist("Frame Times", 100, 0.0, 33.3); // 0 to 33ms
  frame_time_hist.fill(delta_time * 1000.0f);
  ```

#### 3. Advanced Physics & Integration (`intern/physics`)

Hydra's **Numerical Integration** (Gauss-Kronrod, Vegas) solves integrals that are too complex for analytical solutions.

- **Use Case**: Calculating exact buoyancy forces on irregularly shaped meshes (integrating pressure over surface area).
- **Use Case**: **Phase-space Monte Carlo** for simulating particle decay or complex visual effects (e.g., volumetric fog density integration).

### 15.3. Editor Integration

We expose Hydra's data structures to ImGui to visualize the math.

- **Histogram Viewer**: A dedicated Editor Panel that renders `hydra::Histogram1D` as bar charts.
- **Function Plotter**: Use Hydra's functor evaluation to plot procedural generation noise curves before applying them to the terrain.

---

## 16. Dynamic Component System & Game Modules

You asked: _"I was thinking to make shared libraries of components (.dylib) ... and add components option user can search ... is this a good approach?"_

### 16.1. Architectural Analysis: The "Game Module" Approach

**Verdict:** One `.dylib` per component is **NOT** a good approach.
**Recommendation:** Use **Game Modules** (One `.dylib` containing _many_ components).

**Why?**

1.  **OS Overhead:** Loading 500 small `.dylib` files (one for every script) causes massive overhead for the OS loader.
2.  **Symbol Linking:** It makes linking complex dependencies (e.g., specific weapon logic needing specific enemy logic) a nightmare.
3.  **Unity's Approach**: Unity **does not** compile each script to a separate DLL. It compiles _all_ your user scripts into a single `Assembly-CSharp.dll` (or a few Assembly Definition Files).
4.  **Unreal's Approach**: Unreal uses "Modules". A Game is usually one "Primary Game Module" (a single large `.dylib` / `.dll`).

**The Vektor Approach:**
We will implement a **Module System**. One shared library (e.g., `libGameLogic.dylib`) allows you to register _hundreds_ of components at once.

### 16.2. The Registry Architecture

We need a central **Component Registry** that knows which components exist, even if they were loaded dynamically.

**`Source/Runtime/Function/ECS/ComponentRegistry.h`**

```cpp
// A factory function that creates a Component and attaches it to an Entity
using ComponentAttachFn = std::function<void(entt::registry&, entt::entity)>;

class ComponentRegistry {
public:
    static ComponentRegistry& Get() { static ComponentRegistry s; return s; }

    // Plugins call this to tell the Engine a component exists
    void Register(const std::string& name, ComponentAttachFn attacher) {
        factories_[name] = attacher;
    }

    // The Editor calls this to populate the "Add Component" menu
    const std::map<std::string, ComponentAttachFn>& GetAll() { return factories_; }

    // Helper to attach by name
    void Attach(const std::string& name, entt::registry& reg, entt::entity e) {
        if (factories_.count(name)) factories_[name](reg, e);
    }

private:
    std::map<std::string, ComponentAttachFn> factories_;
};
```

### 16.3. The Plugin (Shared Library)

This is what your `.dylib` looks like. It contains the logic for **Gravity**, **Health**, **Inventory**, etc., all in one file.

**`Game/Source/MyGameModule.cpp`**

```cpp
#include "VektorSDK.h" // Includes Registry and Jolt

// 1. Define Valid Components (The Logic)
struct GravityComponent { float force = 9.8f; };

// 2. Define the "Attacher" logic (How to glue it to ECS + Physics)
void AttachGravity(entt::registry& reg, entt::entity e) {
    // A. Add Data to ECS
    reg.emplace<GravityComponent>(e);

    // B. Add Physics to Jolt (if RigidBody exists)
    if(reg.all_of<Rigidbody>(e)) {
        auto& body = reg.get<Rigidbody>(e);
        JoltInterface::SetGravityFactor(body.id, 1.0f);
    }
}

// 3. The Entry Point (Called by Engine when .dylib is loaded)
extern "C" VKT_EXPORT void VektorPluginLoad() {
    auto& registry = Vektor::ComponentRegistry::Get();

    // Register ALL components in this Module
    registry.Register("Gravity", AttachGravity);
    registry.Register("Health", [](auto& r, auto e){ r.emplace<Health>(e, 100); });
    registry.Register("Inventory", [](auto& r, auto e){ r.emplace<Inventory>(e); });
}
```

### 16.4. The Editor Integration ("Add Component" Menu)

How the user sees this in the UI.

**`Source/Editor/Panels/Inspector.cpp`**

```cpp
void DrawAddComponentMenu(entt::registry& reg, entt::entity e) {
    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");

    if (ImGui::BeginPopup("AddComponentPopup")) {
        // Search Bar
        static char search[64] = "";
        ImGui::InputText("Search", search, 64);

        // Iterate Registry
        for (auto& [name, attacher] : ComponentRegistry::Get().GetAll()) {
            // Filter by search
            if (strstr(name.c_str(), search)) {
                if (ImGui::MenuItem(name.c_str())) {
                    attacher(reg, e); // <-- MAGIC HAPPENS HERE
                }
            }
        }
        ImGui::EndPopup();
    }
}
```

### 16.5. Loading the Module (Engine Core)

**`Source/Runtime/Core/PluginSystem.cpp`**

```cpp
void LoadGameModule(const std::string& path) {
    // 1. Open the .dylib / .dll
    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle) { VLOG_ERROR("Failed to load module: {}", dlerror()); return; }

    // 2. Find the Entry Point
    typedef void (*InitFn)();
    InitFn init = (InitFn)dlsym(handle, "VektorPluginLoad");

    // 3. Run Registration
    if (init) init();
    else VLOG_ERROR("Module {} missing VektorPluginLoad symbol", path);
}
```

### Summary of Workflow

1.  **User** writes C++ code for `Gravity`, `Health`, etc.
2.  **User** compiles `libMyGame.dylib`.
3.  **Vektor Engine** starts -> `LoadGameModule("libMyGame.dylib")`.
4.  **Module** calls `Registry::Register("Gravity", ...)` for every component.
5.  **Editor** sees "Gravity" in the Registry map.
6.  **User** types "Grav..." in the UI -> Clicks "Gravity".
7.  **Engine** runs the `AttachGravity` lambda -> Component is added to Entity.

---

## 17. Vektor Native UI (Custom UI System)

You asked: _"I want to create, design and render my own UI rather than using ImGui, in the same way python - C++ integration of UI as Blender."_

### 17.1. Philosophy: Why "Native" UI?

ImGui is great for debugging, but for a **Production Editor** (like Blender), you need:

1.  **Styling**: Full control over rounded corners, gradients, shadows.
2.  **Zoomable**: Vector-based rendering (SDF) that stays crisp at any scale.
3.  **Data-Driven**: Python scripts define the layout, C++ renders it.

### 17.2. The Architecture Stack

We will build a **Hybrid Immediate Mode** system:

- **Python Layer**: Defines the logic (`if selected: ui.button(...)`).
- **C++ Layout Engine**: Calculates `(x, y, w, h)` for every item.
- **GPU Layer**: "Uber Shader" that renders rectangles with SDF effects.

### 17.3. The GPU Layer (SDF Shader)

We don't use textures for buttons. We use math. This allows infinite scaling.

**`Source/Runtime/Render/Shaders/UI/ui_block.frag` (GLSL)**

```glsl
// The "Uber Shader" for all UI elements
in vec2 uv;           // 0..1
in vec2 size_px;      // Size of the box in pixels
in vec4 color;        // Button color
in float radius;      // Corner radius (px)

out vec4 FragColor;

float RoundedRectSDF(vec2 center, vec2 size, float r) {
    return length(max(abs(center) - size + r, 0.0)) - r;
}

void main() {
    // 1. Calculate Distance to edge
    vec2 center = uv * size_px - (size_px * 0.5);
    float dist = RoundedRectSDF(center, size_px * 0.5, radius);

    // 2. Anti-Aliasing (Smooth step at 0.0)
    float alpha = 1.0 - smoothstep(-0.5, 0.5, dist);

    // 3. Drop Shadow (Optional: Offset distance)
    float shadow = 1.0 - smoothstep(0.0, 10.0, dist + 2.0);

    FragColor = vec4(color.rgb, color.a * alpha);
}
```

### 17.4. The C++ Layout Engine

This system takes concise commands ("Row", "Item") and turns them into pixel coordinates.

**`Source/Runtime/UI/UILayout.cpp`**

```cpp
struct UIBlock {
    vec2 pos;
    vec2 size;
    vec4 color;
    std::string text;
};

struct UILayout {
    vec2 cursor = {0, 0};
    float row_height = 0;
    std::vector<UIBlock> draw_list; // What gets sent to GPU

    void Row(float height) {
        cursor.x = 0;
        cursor.y += row_height; // Move down
        row_height = height;
    }

    bool Button(const std::string& label, float width) {
        // 1. Calculate Rect
        UIBlock block;
        block.pos = cursor;
        block.size = {width, row_height};
        block.color = {0.2f, 0.2f, 0.2f, 1.0f}; // Dark Gray
        block.text = label;

        // 2. Advance Cursor
        cursor.x += width + 5.0f; // + padding

        // 3. Store for Rendering
        draw_list.push_back(block);

        // 4. Handle Input (Immediate Mode)
        vec2 mouse = Input::GetMousePos();
        bool hovered = IsInside(mouse, block.pos, block.size);
        if (hovered) block.color = {0.3f, 0.3f, 0.3f, 1.0f}; // Hover Effect

        return hovered && Input::IsMouseClicked();
    }
};
```

### 17.5. The Python Binding (Bridging the Gap)

We expose `UILayout` to Python so scripts can build the UI.

**`Source/Runtime/Function/Scripting/UIBinder.cpp`**

```cpp
void BindUI(py::module& m) {
    py::class_<UILayout>(m, "Layout")
        .def("row", &UILayout::Row)
        .def("button", &UILayout::Button);
}
```

### 17.6. The Python Implementation (Blender Style)

User writes this script to define a Panel.

**`Assets/Scripts/ui/properties_panel.py`**

```python
import vektor
import vektor.ui

class MyCustomPanel(vektor.ui.Panel):
    bl_label = "Object Properties"

    def draw(self, context):
        layout = self.layout
        obj = context.active_object

        layout.row(height=30)
        layout.label("Transform")

        layout.row(height=25)
        # Python logic drives the UI structure!
        if layout.button("Reset Position"):
            obj.location = (0, 0, 0)

        layout.prop(obj, "location")
```

### Summary of Section 17

By implementing this **Hybrid Stack**, you get:

1.  **Beautiful Rendering**: The shader handles rounding/shadows (unlike ImGui's vertex triss).
2.  **Native Feel**: It looks like _your_ engine, not a generic ImGui tool.
3.  **Python Power**: UI logic lives in Python (easy to mod), but rendering lives in C++ (fast).

### 17.7. Example: The Scene Hierarchy Panel (Outliner)

You asked: _"How can I use that to create scene hierarchy panel for my entities?"_

Here is the full implementation of a **Hierarchy Panel** using the architecture above.

**`Assets/Scripts/ui/hierarchy_panel.py`**

```python
import vektor
import vektor.ui

class SCENE_PT_Hierarchy(vektor.ui.Panel):
    bl_label = "Scene Hierarchy"
    bl_region_type = "WINDOW"

    def draw(self, context):
        layout = self.layout
        scene = context.scene

        # 1. Iterate all Entities in the Scene
        for entity in scene.entities:

            # 2. visual Indentation (if we had parenting)
            # layout.indent()

            # 3. Draw a Row for each Entity
            layout.row(height=20)

            # 4. Icon (Cube, Camera, Light)
            icon_id = "OBJECT_DATA"
            if entity.has_component("Camera"): icon_id = "OUTLINER_OB_CAMERA"
            elif entity.has_component("Light"): icon_id = "OUTLINER_OB_LIGHT"

            layout.icon(icon_id)

            # 5. Selectable Label (Handles Click selection)
            # If button returns True, it means it was clicked
            is_selected = (entity == context.active_object)

            if layout.selectable(entity.name, is_selected):
                # Update Engine State
                context.view_layer.objects.active = entity

                # Optional: Ping C++ to highlight in 3D View
                vektor.ops.view3d.select(entity=entity)
```

**New C++ API Features Needed (`UILayout.cpp`)**

To support the above script, we add `icon()` and `selectable()` to our C++ Layout Engine.

```cpp
// In UILayout struct...

void Icon(const std::string& iconID) {
    // Draw a small textured quad (using our UI Shader)
    UIBlock block;
    block.pos = cursor;
    block.size = {row_height, row_height}; // Square
    block.textureID = GetIconTexture(iconID); // Look up "OUTLINER_OB_CAMERA"
    draw_list.push_back(block);

    cursor.x += row_height + 2.0f;
}

bool Selectable(const std::string& label, bool isSelected) {
    // 1. Determine Color
    vec4 color = {0,0,0,0}; // Transparent background
    if (isSelected) color = {0.18f, 0.36f, 0.53f, 1.0f}; // Blender selection blue

    // 2. Draw Background Rect
    UIBlock bg;
    bg.pos = cursor;
    bg.size = {200.0f, row_height}; // Stretch to width
    bg.color = color;
    draw_list.push_back(bg);

    // 3. Draw Text on top
    UIBlock text;
    text.pos = cursor + vec2(5.0f, 0.0f); // Padding
    text.text = label;
    draw_list.push_back(text);

    // 4. Handle Input
    cursor.x += 200.0f; // Advance
    return IsClicked(bg.pos, bg.size);
}
```

---

## 18. Vektor Icon System

You provided reference code from Blender's `interface_icons.cc` and `UI_icons.hh`. We will adopt this robust, scalable architecture for Vektor.

### 18.1. Philosophy

1.  **Unified ID System**: All icons (UI symbols, object types, previews) are referenced by a single `int` ID (e.g., `ICON_OUTLINER_OB_MESH`).
2.  **Hybrid Asset Pipeline**:
    - **SVG**: For scalable UI icons (Mono/Color).
    - **Buffer/Raster**: For thumbnails, previews, and complex images.
3.  **Theme Integration**: "Mono" SVGs are colored at runtime based on the Theme (e.g., Object icons are Orange, Data icons are Green).

### 18.2. Architecture: The `IconRegistry`

Instead of disconnected image files, we use a central registry.

**`Source/Runtime/UI/IconRegistry.h`**

```cpp
enum IconType {
    ICON_TYPE_SVG_MONO,  // Colored by Theme
    ICON_TYPE_SVG_COLOR, // Native colors (e.g. App Logo)
    ICON_TYPE_BUFFER,    // Raw pixel data (Thumbnails)
    ICON_TYPE_PREVIEW,   // Generated at runtime (3D Viewport capture)
};

struct IconData {
    IconType type;
    std::string sourcePath; // "icons/ops/transform.svg"
    void* runtimeCache;     // GPU Texture ID or ImBuf*
};

class IconRegistry {
public:
    static void Init();
    static int GetIconID(const std::string& name);
    static void Draw(int iconID, vec2 pos, float size, vec4 colorOverride = {0});
};
```

### 18.3. Defining Icons (`UI_icons.hh` style)

We use X-Macros to define icons. This allows us to generate Enums and Metadata from one list.

**`Source/Runtime/UI/Icons.def`**

```cpp
//     Name                 Type                File
DEF_ICON(ICON_NONE,         ICON_TYPE_SVG_MONO, "")
DEF_ICON(ICON_CAMERA,       ICON_TYPE_SVG_MONO, "ops/camera.svg")
DEF_ICON(ICON_MESH,         ICON_TYPE_SVG_MONO, "ops/mesh.svg")
DEF_ICON(ICON_FILE,         ICON_TYPE_SVG_MONO, "ops/file.svg")
DEF_ICON(ICON_FOLDER,       ICON_TYPE_SVG_MONO, "ops/folder.svg")
DEF_ICON(ICON_WARNING,      ICON_TYPE_SVG_COLOR, "common/warning.svg")
DEF_ICON(ICON_ERROR,        ICON_TYPE_SVG_COLOR, "common/error.svg")
```

### 18.4. The Rendering Pipeline (`interface_icons.cc` style)

The rendering system handles the complexity of different icon types transparently.

**`Source/Runtime/UI/IconRenderer.cpp`**

```cpp
void IconRegistry::Draw(int iconID, vec2 pos, float size, vec4 colorOverride) {
    IconData& data = GetIconData(iconID);

    // 1. Ensure GPU Resource (Lazy Loading)
    if (!data.runtimeCache) {
        if (data.type == ICON_TYPE_SVG_MONO) {
            data.runtimeCache = LoadSVGTexture(data.sourcePath);
        } else if (data.type == ICON_TYPE_PREVIEW) {
            // Generate preview from 3D scene...
        }
    }

    // 2. Determine Color
    vec4 finalColor = {1, 1, 1, 1};
    if (data.type == ICON_TYPE_SVG_MONO) {
        // Use Theme Color unless overridden
        if (colorOverride.a > 0) {
            finalColor = colorOverride;
        } else {
            finalColor = Theme::GetColor(TH_ICON_DEFAULT);
        }
    }

    // 3. Submit to Draw List (Immediate Mode)
    // We us a "Quad" with the Icon Texture
    UILayout::DrawTextureQuad(data.runtimeCache, pos, size, finalColor);
}
```

### 18.5. Integration with Python

This system powers the `layout.icon()` call we defined in Section 17.

**`Source/Runtime/Function/Scripting/UIBinder.cpp`**

```cpp
// Python: layout.icon("ICON_CAMERA")
void UILayout_Icon(UILayout* self, std::string iconName) {
    int id = IconRegistry::GetIconID(iconName);

    // Draw using the registry
    IconRegistry::Draw(id, self->cursor, self->row_height);

    self->cursor.x += self->row_height + 2.0f; // Advance cursor
}
```

This completes the loop: **Python String -> C++ ID -> IconRegistry Lookup -> Lazy Load -> Render.**

---

## 19. Event System (Blender & Vektor Comparison)

The goal of this section is to explain how user input (Mouse/Keyboard) flows from the Operating System into the Engine, and how we adapt that for Vektor.

### 19.1. Overview (GHOST vs WM)

In Blender, event handling is split into two layers:

1.  **GHOST (Generic Handy Operating System Toolkit)**:
    - **Low-Level**: Hides Win32/Cocoa/X11 details.
    - **Raw Events**: Provides raw key codes, mouse coordinates (screen space), and tablet pressure.
    - **File**: `intern/ghost/GHOST_Types.hh`

2.  **WM (Window Manager)**:
    - **High-Level**: Converts GHOST events into internal `wmEvent` structs.
    - **Keymap**: Handles hotkey mapping (e.g., `Ctrl+Z` -> `ED_UNDO`).
    - **Dispatch**: Routes events to the specific Window, Area, and Region under the mouse.
    - **File**: `source/blender/windowmanager/WM_types.hh`

### 19.2. GHOST: The Platform Layer

Blender defines an abstraction layer so the rest of the engine doesn't know about the OS.

**`intern/ghost/GHOST_Types.hh`**

```cpp
enum GHOST_TEventType {
  GHOST_kEventUnknown = 0,
  GHOST_kEventCursorMove,
  GHOST_kEventButtonDown,
  GHOST_kEventButtonUp,
  GHOST_kEventWheel,
  GHOST_kEventKeyDown,
  GHOST_kEventKeyUp,
  GHOST_kEventQuitRequest,
  // ... (Window resize, Tablet events, etc.)
};

struct GHOST_TEventKeyData {
  GHOST_TKey key;        // e.g. GHOST_kKeyA
  char utf8_buf[6];      // Unicode character
  char is_repeat;        // Auto-repeat flag
};
```

### 19.3. WM: High-Level Semantics

The Window Manager consumes GHOST events and packages them into `wmEvent`. This struct is what Operators see.

**`source/blender/windowmanager/WM_types.hh`**

```cpp
struct wmEvent {
  wmEvent *next, *prev;

  wmEventType type;       // Short code (MOUSEMOVE, EVT_RETURN, 'A'...)
  short val;              // KM_PRESS, KM_RELEASE, KM_NOTHING
  int xy[2];              // Mouse position (Screen Coordinates)
  int mval[2];            // Mouse position (Region Relative) - Critical for UI!

  wmEventModifierFlag modifier; // KM_SHIFT, KM_CTRL, KM_ALT

  // Tablet pressure/tilt data
  wmTabletData tablet;
};
```

### 19.4. The Event Loop Lifecycle

The function `wm_event_do_handlers` is the heart of Blender's interaction model. It decides _who_ gets the event.

1.  **Poll GHOST**: `GHOST_System::processEvents()` fills a queue.
2.  **Convert**: `wm_event_add_ghostevent` converts to `wmEvent` and adds to `wmWindow.event_queue`.
3.  **Dispatch (`wm_event_do_handlers`)**:
    - **Global Handlers**: Checks if a modal operator (like File Select) eats the event.
    - **Window Handlers**: Checks window-level keymaps.
    - **Screen/Area/Region**:
      - Determines which Area/Region the mouse is over.
      - Converts `xy` (screen) to `mval` (region local).
      - Iterates handlers (`wmEventHandler`) for that region.
      - **UI Handler**: Checks if a button was clicked.
      - **Keymap Handler**: Checks if a hotkey matches.
      - **Gizmo Handler**: Checks if a 3D widget was clicked.

### 19.5. Vektor Implementation

For Vektor, we will adopt this layered approach:

1.  **Platform Layer**: Use **GLFW** as our "GHOST". It provides similar valid/window abstractions.
2.  **Event Bus**: Create a `Vektor::Event` struct similar to `wmEvent`.
3.  **Dispatch Loop**:
    - Poll GLFW.
    - Construct `Vektor::Event`.
    - Pass to `Editor::OnEvent(e)`.
    - Editor checks ImGui (UI) -> then check active Tool -> then check Camera.

```cpp
// Vektor Event Loop (Pseudocode)

void Application::Run() {
    while (!window.ShouldClose()) {
        glfwPollEvents(); // GHOST equivalent

        // 1. Input Processing
        ProcessInput();

        // 2. Logic Update
        // 3. Render
    }
}

void OnGlfwKey(int key, int scancode, int action, int mods) {
    Vektor::Event e;
    e.type = ConvertKey(key);
    e.val = (action == GLFW_PRESS) ? Vektor::PRESS : Vektor::RELEASE;

    // Dispatch to Event Bus
    Vektor::EventBus::Publish(e);
}
```

---

## 20. Low-Level Memory System (Guarded & Pools)

Blender achieves stability and performance not by using standard `malloc`/`new`, but by routing _all_ allocations through a centralized, debuggable memory system. Vektor will adopt this strict discipline.

### 20.1. The "Guarded" Allocator (`MEM_*`)

In Blender, you never call `new` or `malloc` directly. You use the **Guarded Allocator** (`intern/guardedalloc`).

**Key Features we will copy:**

1.  **Padding (Guard Bytes)**: Every allocation has `0xDEADBEEF` written before and after the user memory. When freeing, we check if these are intact. If not, we know a generic buffer overflow occurred.
2.  **Leak Detection**: All blocks are stored in a global linked list. On exit, we print the list of unfreed blocks.
3.  **Usage Tracking**: We can see real-time memory stats per module (e.g., "Rendering: 500MB", "Physics: 20MB").

**Blender C API (`MEM_guardedalloc.h`)**:

```c
// Allocation with a "Name" for debugging
void* ptr = MEM_mallocN(1024, "MeshVertexBuffer");

// Freeing (checks guards implicitly)
MEM_freeN(ptr);
```

**Vektor Implementation (`Source/Runtime/Core/Memory/GuardedAllocator.h`)**:

```cpp
namespace Vektor::Memory {
    template<typename T, typename... Args>
    T* New(const char* debugName, Args&&... args) {
        // 1. Allocate Body + Header + Footer
        size_t totalSize = sizeof(T) + sizeof(BlockHeader) + sizeof(BlockFooter);
        void* raw = malloc(totalSize);

        // 2. Setup Guards
        BlockHeader* header = (BlockHeader*)raw;
        header->name = debugName;
        header->magic = 0xDEADBEEF;

        // 3. Construct Object
        T* ptr = new ((char*)raw + sizeof(BlockHeader)) T(std::forward<Args>(args)...);
        return ptr;
    }

    template<typename T>
    void Delete(T* ptr) {
        // 1. Check consistency
        BlockHeader* header = ((BlockHeader*)ptr) - 1;
        assert(header->magic == 0xDEADBEEF && "Memory Corruption Detected!");

        // 2. Destruct & Free
        ptr->~T();
        free(header);
    }
}
```

### 20.2. Memory Pools (`BLI_mempool`)

For small, frequently created/destroyed objects (like Particles, Nodes, or Events), `malloc` is too slow and causes fragmentation. Blender uses `BLI_mempool`.

**How it works:**

- Allocates global "Chunks" (e.g., 64KB pages).
- Slices chunks into fixed-size "nodes".
- `Alloc` simply pops a node from the free list (O(1)).
- `Free` pushes it back to the free list (O(1)).

**Blender Usage (`source/blender/blenlib/BLI_mempool.h`):**

```c
// Create pool for 1000 'Particle' structs per chunk
BLI_mempool *pool = BLI_mempool_create(sizeof(Particle), 1000, 512, BLI_MEMPOOL_NOP);

// Fast alloc
Particle *p = BLI_mempool_alloc(pool);

// Iteration (Super cache friendly)
BLI_mempool_iter iter;
BLI_mempool_iternew(pool, &iter);
while ((p = BLI_mempool_iterstep(&iter))) {
    // update particle...
}
```

**Vektor Implementation**:
We will use this for our **ECS Components**.

- Each `ComponentPool<T>` in EnTT will use a custom `Vektor::Memory::PoolAllocator` backend that mimics `BLI_mempool`.

### 20.3. The Arena (Linear Allocator) (`BLI_memarena`)

Used for "Frame Scratchpad" memory. Things that only exist for one frame (e.g., UILayout commands, Render commands).

**Concept:**

- One massive pointer `cursor`.
- `Alloc(size)` -> `return cursor; cursor += size;`
- `Reset()` -> `cursor = start;`
- **Zero overhead** deallocation.

**Vektor Usage**:

- **RenderGraph**: Pass objects and commands are allocated in a `FrameArena`.
- **UI**: ImGui vertex buffers are allocated in a `UIArena`.
- At the end of `Application::Update()`, we call `FrameArena.Reset()`.

---

Resources :
https://chatgpt.com/c/698e98e7-9ed0-8324-804d-e15e49886afa

https://chatgpt.com/c/698c981b-3cd0-83a3-a2ba-5838650d9788

https://medium.com/@EDBCBlog/designing-an-event-driven-imgui-architecture-from-zero-to-hero-no-phd-required-82290c082c6a

https://github.com/NodeppOfficial/nodepp

https://en.cppreference.com/w/cpp/language/coroutines.html

