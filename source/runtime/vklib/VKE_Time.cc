#include "VKE_Time.h"
#include <chrono>

namespace vektor
{
double VKE_time_now_seconds(void)
{
    return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
}
} // namespace vektor