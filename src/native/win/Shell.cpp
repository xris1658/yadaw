#if _WIN32

#include "native/Shell.hpp"

#include "native/Library.hpp"

#include <windows.h>

#include <mutex>

namespace YADAW::Native
{
QString fileBrowserName;

std::once_flag getFileBrowserNameFlag;

QString getFileBrowserName()
{
    std::call_once(getFileBrowserNameFlag,
        []()
        {
            const WCHAR defaultName[] = L"File Explorer";
            YADAW::Native::Library explorerLib("explorer.exe");
            if(explorerLib.loaded())
            {
                auto explorerHandle = reinterpret_cast<HMODULE>(explorerLib.handle());
                LONG_PTR lParam = 0;
                WCHAR* namePtr = nullptr;
                auto loadStringRet = LoadStringW(explorerHandle, 6020, reinterpret_cast<WCHAR*>(&namePtr), 0);
                if(loadStringRet)
                {
                    fileBrowserName = QString::fromWCharArray(namePtr, loadStringRet);
                    return;
                }
            }
            fileBrowserName = QString::fromWCharArray(defaultName);
        }
    );
    return fileBrowserName;
}

constexpr wchar_t prefix[] = L"/select,";

void locateFileInExplorer(const QString& path)
{
    auto prefixLength = std::size(prefix) - 1;
    std::wstring arg(prefixLength + path.size(), L'\0');
    std::wcscpy(arg.data(), prefix);
    path.toWCharArray(arg.data() + prefixLength);
    std::replace(arg.begin() + prefixLength, arg.end(), L'/', L'\\');
    ShellExecuteW(NULL, L"open", L"explorer.exe", arg.data(), NULL, SW_SHOWNORMAL);
}
}

#endif