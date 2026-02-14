
#include <cstdlib>
#include <cstring>

#include <csignal>
#include <iostream>


#include "creator_args.hh"
#include "creator_global.hh"

#include "../../intern/clog/COG_log.hh"
#include "../runtime/vklib/VKE_assert.h"

#include "../runtime/windowmanager/wm_event.hh"
#include "../runtime/windowmanager/wm_keymap.h"
#include "../runtime/windowmanager/wm_system.h"

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "Application");

// namespace vektor

int main(int          argc,
         const char** argv)
{
    CLG_logref_init(CLG_LogRef_App);
    CLG_level_set(CLG_LEVEL_TRACE);

    CLOG_INFO(CLG_LogRef_App, "APPLICATION STARTED");

    vektor::creator::G.is_break   = false;
    vektor::creator::G.background = false;

    vektor::creator::Args args;
    vektor::creator::main_args_setup(args);
    int result = args.parse(argc, argv);

    VKE_assert(argc > 0);
    VKE_assert_msg(argv != nullptr, "Argument vector cannot be null");

    if (result != 0)
        return result;

    if (vektor::creator::G.project_file)
    {
        CLOG_INFO(CLG_LogRef_App, "Loading project: %s", vektor::creator::G.project_file);

        CLOG_INFO(CLG_LogRef_App, "Engine initialized.");
        CLOG_INFO(CLG_LogRef_App, "Running loop...");

        std::cout << "--- vektor-Style Event System Demo ---\n";

        vektor::init_wm();

        vektor::vkContext ctx;

        vektor::setup_operators();
        vektor::setup_keymap();

        vektor::simulate_vektor_input();

        std::cout << "\n[System] Processing Events...\n";

        vektor::G_WM->process_events((vektor::vkContext*)&ctx);

        std::cout << "\n--- End of Demo ---\n";

        for (int i = 0; i < 3; ++i)
        {
            CLOG_INFO(CLG_LogRef_App, "Frame %d processed.", i);
        }
        CLOG_INFO(CLG_LogRef_App, "Engine shut down.");
    }
    else
    {
        CLOG_ERROR(CLG_LogRef_App, "No project file specified. Use 'vektor [path/to/project.vproj]'");
    }
    return 0;
}