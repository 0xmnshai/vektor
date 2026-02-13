
#ifndef LIBMV_THREADING_THREADING_H_
#define LIBMV_THREADING_THREADING_H_

#include <pthread.h>
#include <mutex>

namespace libmv
{

using mutex              = std::mutex;
using scoped_lock        = std::unique_lock<std::mutex>;
using condition_variable = std::condition_variable;

} // namespace libmv

#endif