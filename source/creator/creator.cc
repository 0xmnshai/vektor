
#include <cstdlib>
#include <cstring>

#include <csignal>

#include "../../intern/clog/COG_log.hh"
#include "../runtime/vklib/VKE_assert.h"

#include "creator_args.hh"
#include "creator_global.hh"
#include "draw/PY_imgui.hh"

// testing python UI
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLUT/glut.h>

#include <pylifecycle.h>
#include <pythonrun.h>

#include "../runtime/windowmanager/wm_event.h"
#include "./imgui/UI_theme.hh"
#include "wm_system.h"

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "Application");

static void glfw_error_callback(int         error,
                                const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int          argc,
         const char** argv)
{
    CLG_logref_init(CLG_LogRef_App);
    CLG_level_set(CLG_LEVEL_TRACE);

    CLOG_INFO(CLG_LogRef_App, "APPLICATION STARTED");

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to initialize GLFW");
        return -1;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    vektor::creator::G.is_break   = false;
    vektor::creator::G.background = false;

    vektor::creator::Args args;
    vektor::creator::main_args_setup(args);
    int result = args.parse(argc, argv);

    VKE_assert(argc > 0);
    VKE_assert_msg(argv != nullptr, "Argument vector cannot be null");

    if (result != 0)
        return result;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vektor Style ImGui + Python", NULL, NULL);

    if (!window)
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to initialize GLAD");
        return -1;
    }

    // key callback
    glfwSetKeyCallback(window,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods)
                       {
                           if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                           {
                               glfwSetWindowShouldClose(window, true);
                           }

                           vektor::wmEvent event;

                           // Map GLFW keys to Vektor's wmEventType
                           if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
                           {
                               event.type = static_cast<vektor::wmEventType>(vektor::EVT_AKEY + (key - GLFW_KEY_A));
                           }
                           else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
                           {
                               event.type = static_cast<vektor::wmEventType>(vektor::EVT_ZEROKEY + (key - GLFW_KEY_0));
                           }
                           else if (key == GLFW_KEY_ESCAPE)
                           {
                               event.type = vektor::EVT_ESCKEY;
                           }
                           else if (key == GLFW_KEY_SPACE)
                           {
                               event.type = vektor::EVT_SPACEKEY;
                           }
                           else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
                           {
                               event.type = vektor::EVT_LEFTCTRLKEY;
                           }
                           else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
                           {
                               event.type = vektor::EVT_LEFTSHIFTKEY;
                           }
                           else if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT)
                           {
                               event.type = vektor::EVT_LEFTALTKEY;
                           }
                           else
                           {
                               event.type = vektor::EVENT_NONE;
                           }

                           if (event.type != vektor::EVENT_NONE)
                           {
                               event.value = (action == GLFW_RELEASE) ? vektor::EVT_RELEASE : vektor::EVT_PRESS;

                               // Apply modifiers
                               if (mods & GLFW_MOD_SHIFT)
                                   event.modifiers |= vektor::EVT_SHIFT;
                               if (mods & GLFW_MOD_CONTROL)
                                   event.modifiers |= vektor::EVT_CTRL;
                               if (mods & GLFW_MOD_ALT)
                                   event.modifiers |= vektor::EVT_ALT;

                               vektor::G_WM->push_event(event);
                           }
                       });

    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow* window, int button, int action, int mods)
                               {
                                   vektor::wmEvent event;
                                   if (button == GLFW_MOUSE_BUTTON_LEFT)
                                       event.type = vektor::LEFTMOUSE;
                                   else if (button == GLFW_MOUSE_BUTTON_RIGHT)
                                       event.type = vektor::RIGHTMOUSE;
                                   else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
                                       event.type = vektor::MIDDLEMOUSE;
                                   else
                                       event.type = vektor::EVENT_NONE;

                                   if (event.type != vektor::EVENT_NONE)
                                   {
                                       event.value = (action == GLFW_RELEASE) ? vektor::EVT_RELEASE : vektor::EVT_PRESS;

                                       double x_pos, y_pos;
                                       glfwGetCursorPos(window, &x_pos, &y_pos);
                                       event.x = static_cast<int>(x_pos);
                                       event.y = static_cast<int>(y_pos);

                                       vektor::G_WM->push_event(event);
                                   }
                               });

    glfwSetCursorPosCallback(window,
                             [](GLFWwindow* window, double x_pos, double y_pos)
                             {
                                 vektor::wmEvent event;
                                 event.type = vektor::EVT_MOUSEMOVE;
                                 event.x    = x_pos;
                                 event.y    = y_pos;
                                 vektor::G_WM->push_event(event);
                             });

    glfwSetScrollCallback(window,
                          [](GLFWwindow* window, double xoffset, double yoffset)
                          {
                              vektor::wmEvent event;

                              if (yoffset > 0)
                              {
                                  event.type  = vektor::EVT_WHEELUP;
                                  event.value = yoffset;
                                  vektor::G_WM->push_event(event);
                              }
                              else if (yoffset < 0)
                              {
                                  event.type  = vektor::EVT_WHEELDOWN;
                                  event.value = yoffset;
                                  vektor::G_WM->push_event(event);
                              }
                          });

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    vektor::setup_vektor_theme();

    // Register our built-in module
    if (PyImport_AppendInittab("vektor_ui", &PyInit_vektor_ui) == -1)
    {
        CLOG_ERROR(CLG_LogRef_App, "Failed to register python  module");
        return 1;
    }

    vektor::init_wm();
    vektor::setup_operators();
    vektor::setup_keymap();

    Py_Initialize();

    const std::string py_script_folder = "/Users/lazycodebaker/Documents/Dev/CPP/vektor/scripts/ui";

    PyRun_SimpleString("import sys");

    PyRun_SimpleString(("sys.path.append('" + py_script_folder + "')").c_str());

    // Import the 'main' module (our script)
    PyObject* pName   = PyUnicode_DecodeFSDefault("main");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load 'main.py'\n");
        return 1;
    }

    // Get the 'draw' function
    PyObject* pFunc = PyObject_GetAttrString(pModule, "draw");
    if (pFunc && PyCallable_Check(pFunc))
    {
        // Good
    }
    else
    {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function 'draw'\n");
    }

    // 8. Main Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Process Vektor events (and log them)
        vektor::G_WM->process_events((vektor::vkContext*)window);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // A. Render WindowManager (Layouts)
        vektor::G_WM->on_render();

        // B. Render Python UI
        if (pFunc)
        {
            PyObject* pValue = PyObject_CallObject(pFunc, NULL);
            if (pValue != NULL)
            {
                Py_DECREF(pValue);
            }
            else
            {
                PyErr_Print();
            }
        }

        // Reload helper (optional, for dev)
        // PyRun_SimpleString("import main; import importlib; importlib.reload(main)");

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Viewports
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);
    Py_Finalize();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (vektor::creator::G.project_file)
    {
        CLOG_INFO(CLG_LogRef_App, "Project: %s", vektor::creator::G.project_file);
    }
    else
    {
        CLOG_ERROR(CLG_LogRef_App, "No project file specified. Use 'vektor [path/to/project.vproj]' log disabled");
    }
    return 0;
}