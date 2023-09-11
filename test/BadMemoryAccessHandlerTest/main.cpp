#include "native/Native.hpp"

int main()
{
    YADAW::Native::setBadMemoryAccessHandler();
    volatile int* ptr = nullptr;
    *ptr = 0;
}