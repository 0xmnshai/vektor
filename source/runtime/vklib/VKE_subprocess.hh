#pragma once

#if defined(__linux__)
#define BLI_SUBPROCESS_SUPPORT 0
#else
#define BLI_SUBPROCESS_SUPPORT 1
#endif

#if BLI_SUBPROCESS_SUPPORT

#include <semaphore.h>
#include <csignal>

#include "VKE_span.hh"
#include "VKE_string_ref.hh"

#include "VKE_utility_mixins.hh"

namespace vektor
{
namespace vklib
{
class VektorSubProcess : public NonCopyable
{
    pid_t pid;

    ~VektorSubProcess()
    {
        if (pid)
        {
            kill(pid, SIGKILL);
        }
    }

    bool create(vklib::Span<vklib::StringRefNull> args);

    bool is_running() const;
};

class SharedMemory : public NonCopyable
{
private:
    const char* name_;
    void*       data_;
    size_t      data_size_;
    bool        is_owner_;

public:
    SharedMemory(const char* name,
                 size_t      size,
                 bool        is_owner);
    ~SharedMemory();

    void*  get_data() const { return data_; }

    size_t get_size() const { return data_size_; }
};

class SharedSemaphore : NonCopyable
{
private:
    const char* name_;
    sem_t*      handle_;
    bool        is_owner_;

public:
    SharedSemaphore(const char* name,
                    bool        is_owner);
    ~SharedSemaphore();

    void increment();
    void decrement();
    bool try_decrement(int wait_ms = 0);
};
} // namespace vklib
} // namespace vektor

#endif