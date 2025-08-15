#ifndef YADAW_PCH_WINDOWSAPI
#define YADAW_PCH_WINDOWSAPI

// Windows API
#if WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <winreg.h>
#include <winerror.h>
#include <avrt.h>
#include <dwmapi.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <realtimeapiset.h>
#include <timezoneapi.h>

#include <combaseapi.h>
#include <ShlObj.h>
#include <synchapi.h>

// C++/WinRT API
#include "native/win/winrt/Forward.hpp"
#include <../include/winrt/base.h>
#include <../include/winrt/Windows.Foundation.h>
#include <../include/winrt/Windows.Foundation.Collections.h>
#include <../include/winrt/Windows.Devices.Enumeration.h>
#include <../include/winrt/Windows.Devices.Midi.h>
#include <../include/winrt/Windows.Media.h>
#include <../include/winrt/Windows.Media.Audio.h>
#include <../include/winrt/Windows.Media.Devices.h>
#include <../include/winrt/Windows.Media.MediaProperties.h>
#include <../include/winrt/Windows.Media.Render.h>
#include <../include/winrt/Windows.Storage.Streams.h>
#endif

#endif // YADAW_PCH_WINDOWSAPI
