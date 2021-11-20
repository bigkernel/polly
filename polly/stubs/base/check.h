#pragma once

#include <cstdlib>
#include <cassert>

#include "stubs/base/attributes.h"
#include "stubs/base/const.h"

namespace polly {
namespace check_internal {
void SafeWriteToStderr(const char* file, int line, const char* fmt, ...);
} // namespace check_internal
} // namespace polly

#define POLLY_MESSAGE(fmt, ...)  do {                                       \
  constexpr const char* basename = polly::ConstBasename(__FILE__);          \
  check_internal::SafeWriteToStderr(basename, __LINE__, fmt, __VA_ARGS__);  \
} while (0)

#define POLLY_CHECK(exp, msg)                           \
  if (POLLY_EXPECT_FALSE(!(exp))) {                     \
    POLLY_MESSAGE("Check %s failed: %s", #exp, msg);    \
    std::abort();                                       \
  }

#if defined(NDEBUG)
# define POLLY_CONST_ASSERT(exp) \
    (false ? static_cast<void>(exp) : static_cast<void>(0))
#else
# define POLLY_CONST_ASSERT(exp) \
    (POLLY_EXPECT_TRUE(exp) ? static_cast<void>(0) : [] { assert(false && #exp); }())
#endif