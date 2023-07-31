#ifndef YADAW_TEST_COMMON_DISABLESTREAMBUFFER
#define YADAW_TEST_COMMON_DISABLESTREAMBUFFER

#include <cstdio>

inline void disableStdOutBuffer()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
}

#endif // YADAW_TEST_COMMON_DISABLESTREAMBUFFER
