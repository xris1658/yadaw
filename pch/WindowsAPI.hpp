#ifndef YADAW_PCH_WINDOWSAPI
#define YADAW_PCH_WINDOWSAPI

#define NOMINMAX

// Windows API
#if WIN32
#define NOMINMAX
#include <Windows.h>
#include <winreg.h>
#include <winerror.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <realtimeapiset.h>
#include <timezoneapi.h>
#include <combaseapi.h>
#include <ShlObj_core.h>
#include <synchapi.h>

#include <MemoryBuffer.h>

// C++/WinRT API
#include "native/win/winrt/Forward.hpp"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Midi.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.Devices.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Render.h>
#include <winrt/Windows.Storage.Streams.h>
#endif

#endif // YADAW_PCH_WINDOWSAPI
