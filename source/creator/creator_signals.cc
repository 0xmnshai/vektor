#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "creator_global.hh"
#include "creator_intern.hh"

#include "../runtime/vklib/VKE_fileops_c.h"

using namespace vektor::creator;

#ifndef FILE_MAX
#define FILE_MAX PATH_MAX
#endif

void VKT_system_backtrace_with_os_info(FILE*       fp,
                                       const void* os_info)
{
    if (fp)
    {
        fprintf(fp, "Backtrace not implemented\n");
    }
}

namespace vektor
{
namespace creator
{

static void sig_handle_fpe(int)
{
    std::cerr << "Floating point exception\n";
    exit(1);
}

static void sig_handle_ill(int)
{
    std::cerr << "Illegal instruction\n";
    exit(1);
}

static void sig_handle_segfault(int)
{
    std::cerr << "Segmentation fault\n";
    exit(1);
}

static void sig_handle_vektor_esc(int sig)
{
    G.is_break = true;

    if (sig == 2)
    {
        static int count = 0;

        if (count)
        {
            printf("\nvektor killed\n");
            exit(2);
        }
        printf("\nSent an internal break event. Press ^C again to kill vektor\n");
        count++;
    }
}

static void crashlog_file_generate(const char* filepath,
                                   const void* os_info)
{

    FILE* fp;
    char  header[512];

    printf("Generating crash log at %s\n", filepath);
    fflush(stdout);

    errno = 0;

    fp    = vklib::VKE_fopen(filepath, "wb");
    if (fp == nullptr)
    {
        fprintf(stderr, "Unable to save '%s': %s\n", filepath, errno ? strerror(errno) : "Unknown error opening file");
    }
    else
    {

        fputs("\n# backtrace\n", fp);
        VKT_system_backtrace_with_os_info(fp, os_info);

#ifdef WITH_PYTHON

        VKPY_python_backtrace(fp);
#endif

        fclose(fp);
    }
}

static void sig_cleanup_and_terminate(int signum)
{

    signal(signum, SIG_DFL);
    raise(signum);
    exit(signum);
}

static void sig_handle_crash_fn(int signum)
{
    char filepath_crashlog[FILE_MAX];

    crashlog_file_generate(filepath_crashlog, nullptr);
    sig_cleanup_and_terminate(signum);
}

static void sig_handle_abort(int) {}

void        main_signal_setup()
{
    if (app_state.signal.use_crash_handler)
    {
        signal(SIGSEGV, sig_handle_crash_fn);
    }

    if (app_state.signal.use_abort_handler)
    {
        signal(SIGABRT, sig_handle_abort);
    }
}

} // namespace creator
} // namespace vektor