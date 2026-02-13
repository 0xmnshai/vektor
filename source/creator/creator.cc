
#include <cstdlib>
#include <cstring>

#include <csignal>

#include "creator_args.h"
#include "creator_global.h"
#include "creator_intern.h"

#include "../../intern/clog/COG_log.hh"

CLG_LOGREF_DECLARE_GLOBAL(CLG_LogRef_App,
                          "Application");

namespace vektor
{
namespace creator
{
ApplicationState app_state = []()
{
    ApplicationState app_state{};
    app_state.signal.use_crash_handler  = true;
    app_state.signal.use_abort_handler  = true;
    app_state.exit_code_on_error.python = 0;
    app_state.main_arg_deferred         = nullptr;
    return app_state;
}();
} // namespace creator
} // namespace vektor

int main(int          argc,
         const char** argv)
{
    CLG_logref_init(CLG_LogRef_App);
    CLG_level_set(CLG_LEVEL_INFO);

    CLOG_INFO(CLG_LogRef_App, "APPLICATION STARTED");

    vektor::creator::G.is_break   = false;
    vektor::creator::G.background = false;

    vektor::creator::Args args;
    vektor::creator::main_args_setup(args);

    int result = vektor::creator::main_args_handle(argc, argv);
    if (result != 0)
        return result;

    if (vektor::creator::G.project_file)
    {
        CLOG_INFO(CLG_LogRef_App, "Loading project: %s", vektor::creator::G.project_file);

        CLOG_INFO(CLG_LogRef_App, "Engine initialized.");
        CLOG_INFO(CLG_LogRef_App, "Running loop...");

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