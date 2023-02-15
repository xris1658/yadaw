#include "WinRTApartment.hpp"

#include <winrt/base.h>

namespace YADAW::Native
{
WinRTApartment::WinRTApartment()
{
    winrt::init_apartment();
}

void WinRTApartment::init()
{
    static WinRTApartment apartment;
}

WinRTApartment::~WinRTApartment()
{
    winrt::uninit_apartment();
}
}