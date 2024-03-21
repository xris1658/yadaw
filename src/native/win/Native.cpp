#if _WIN32

#include "native/Native.hpp"

#include "util/Base.hpp"
#include "util/FixedSizeMemoryBlock.hpp"

#include <QList>

#include <ShlObj_core.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <timezoneapi.h>
#include <realtimeapiset.h>

#include <array>
#include <bitset>
#include <charconv>
#include <chrono>
#include <cstring>
#include <mutex>
#include <stdexcept>

namespace YADAW::Native
{
namespace Impl
{
using OneHunderdNanoSecondInterval = std::chrono::duration<
    std::chrono::steady_clock::rep,
    std::ratio<1, 10000000>
>;

template<int CSIDL>
class SHGetFolderHelper
{
private:
    SHGetFolderHelper()
    {
        auto getFolderResult = SHGetFolderPathW(
            nullptr,
            CSIDL,
            NULL,
            SHGFP_TYPE_CURRENT,
            path_);
        if(getFolderResult != S_OK)
        {
            path_[0] = '\0';
        }
    }
public:
    static SHGetFolderHelper& getFolderHelper()
    {
        static SHGetFolderHelper ret;
        return ret;
    }
    const wchar_t* operator()()
    {
        return path_;
    }
private:
    wchar_t path_[MAX_PATH];
};

template<int CSIDL>
const QString& getFolderAsWCharArray()
{
    static auto ret = QString::fromWCharArray(SHGetFolderHelper<CSIDL>::getFolderHelper()());
    return ret;
}

std::uint64_t procMask()
{
    std::uint64_t procMask = 0;
    std::uint64_t sysMask = 0;
    if(GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask))
    {
        return procMask;
    }
    return 0;
}

std::int64_t getQPF()
{
    LARGE_INTEGER ret;
    QueryPerformanceFrequency(&ret);
    return ret.QuadPart;
}

std::int64_t qpf()
{
    static auto ret = getQPF();
    return ret;
}
}

const QString& appDataFolder()
{
    return Impl::getFolderAsWCharArray<CSIDL_APPDATA>();
}

const QString& programFilesFolder()
{
    return Impl::getFolderAsWCharArray<CSIDL_PROGRAM_FILES>();
}

const QString& localAppDataFolder()
{
    return Impl::getFolderAsWCharArray<CSIDL_LOCAL_APPDATA>();
}

void openSpecialCharacterInput()
{
    ShellExecuteW(nullptr, L"open", L"charmap.exe", nullptr, nullptr, SW_NORMAL);
}

void showFileInExplorer(const QString& path)
{
    constexpr char part1Raw[] = "explorer /select,";
    constexpr auto size = YADAW::Util::stringLength(part1Raw);
    QString command = part1Raw;
    for(int i = 0; i < path.size(); ++i)
    {
        command[int(i + size)] = path[i] == '/'? YADAW::Native::PathSeparator: path[i];
    }
    auto commandWide = command.toStdWString();
    ShellExecuteW(nullptr, L"open", commandWide.data(), nullptr, nullptr, SW_NORMAL);
}

int getProcessCPUCoreCount()
{
    auto procMask = Impl::procMask();
    auto bitset = reinterpret_cast<std::bitset<sizeof(procMask) * CHAR_BIT>*>(&procMask);
    return bitset->count();
}

void setThreadPriorityToTimeCritical()
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}

bool isDebuggerPresent()
{
    return IsDebuggerPresent();
}

QString errorMessageFromErrorCode(ErrorCodeType errorCode)
{
    TCHAR* rawErrorString = nullptr;
    auto messageLength = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorCode, 0, (LPTSTR)&rawErrorString, 0, nullptr);
    if(messageLength == 0)
    {
        auto handle = LoadLibrary(TEXT("avrt.dll"));
        if(handle)
        {
            messageLength = FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                handle, errorCode, 0, (LPTSTR)&rawErrorString, 0, nullptr
            );
            FreeLibrary(handle);
        }
    }
    if(messageLength == 0)
    {
        errorCode = GetLastError();
    }
    else
    {
        QString ret;
        if constexpr(std::is_same_v<TCHAR, char>)
        {
            // supress the false warning that mistakenly treats `TCHAR` and `char`
            // as different types
            auto ptr = reinterpret_cast<char*>(rawErrorString);
            ret = QString(ptr);
        }
        else if constexpr(std::is_same_v<TCHAR, wchar_t>)
        {
            // ditto
            auto ptr = reinterpret_cast<wchar_t*>(rawErrorString);
            ret = QString::fromWCharArray(ptr);
        }
    }
    LocalFree(rawErrorString);
    return {};
}

QString getProductVersion(const QString& path)
{
    wchar_t buffer[MAX_PATH];
    buffer[path.size()] = L'\0';
    path.toWCharArray(buffer);
    auto fileVersionInfoSize = GetFileVersionInfoSizeW(buffer, nullptr);
    if(fileVersionInfoSize)
    {
        YADAW::Util::FixedSizeMemoryBlock fileInfoBuffer(fileVersionInfoSize);
        GetFileVersionInfoW(buffer, 0, fileVersionInfoSize,
            reinterpret_cast<void*>(fileInfoBuffer.data()));
        const wchar_t* subBlock = L"\\";
        VS_FIXEDFILEINFO* pFileInfo = nullptr;
        UINT pFileInfoSize = 0;
        auto queryValueResult = VerQueryValueW(reinterpret_cast<const void*>(fileInfoBuffer.data()),
            subBlock, reinterpret_cast<void**>(&pFileInfo), &pFileInfoSize);
        if (queryValueResult != 0)
        {
            QString format("%1.%2.%3.%4");
            auto versions = reinterpret_cast<std::uint16_t*>(&(pFileInfo->dwProductVersionMS));
            if constexpr(SystemEndian == YADAW::Util::Endian::LittleEndian)
            {
                return format.arg(versions[1]).arg(versions[0]).arg(versions[3]).arg(versions[2]);
            }
            else if constexpr(SystemEndian == YADAW::Util::Endian::BigEndian)
            {
                return format.arg(versions[0]).arg(versions[1]).arg(versions[2]).arg(versions[3]);
            }
        }
    }
    return {};
}

void sleepFor(std::chrono::steady_clock::duration duration)
{
    static auto handle = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER timeIn10e7;
    timeIn10e7.QuadPart = static_cast<std::int64_t>(
        std::chrono::duration_cast<Impl::OneHunderdNanoSecondInterval>(duration).count()
    );
    SetWaitableTimerEx(handle, &timeIn10e7, 0, NULL, NULL, NULL, 0);
    WaitForSingleObject(handle, INFINITE);
}

std::once_flag defaultPluginListFlag;

const std::vector<QString>& defaultPluginDirectoryList()
{
    static std::vector<QString> ret;
    std::call_once(defaultPluginListFlag, [&list = ret]() mutable
    {
        list.reserve(6);
        auto programFilesPath = YADAW::Native::programFilesFolder();
        if(!programFilesPath.isEmpty())
        {
            // Vestifal
            list.emplace_back(QString(programFilesPath).append("\\Steinberg\\VstPlugins"));
            list.emplace_back(QString(programFilesPath).append("\\VstPlugins"));
            // VST3
            list.emplace_back(QString(programFilesPath).append("\\Common Files\\VST3"));
            // CLAP
            list.emplace_back(QString(programFilesPath).append("\\Common Files\\CLAP"));
        }
        auto localAppDataPath = YADAW::Native::localAppDataFolder();
        if(!localAppDataPath.isEmpty())
        {
            list.emplace_back(QString(localAppDataPath).append("\\Programs\\Common\\VST3"));
            // CLAP
            list.emplace_back(QString(localAppDataPath).append("\\Programs\\Common\\CLAP"));
        }
        auto clapPath = std::getenv("CLAP_PATH");
        if(clapPath)
        {
            QString clapPathAsQString(clapPath);
            auto results = clapPathAsQString.split(';');
            for(const auto& result: results)
            {
                if(result.size() != 0)
                {
                    list.emplace_back(result);
                }
            }
        }
    });
    return ret;
}

void locateFileInExplorer(const QString& path)
{
    std::wstring arg(L"/select,");
    arg.append(path.toStdWString());
    ShellExecuteW(NULL, L"open", L"explorer.exe", arg.data(), NULL, SW_SHOWNORMAL);
}

void mySegFaultHandler()
{
    std::fprintf(stderr, "Access violation\n");
    std::terminate();
}

LONG WINAPI accessViolationHandler(PEXCEPTION_POINTERS ptr) noexcept
{
    if(ptr->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
        ptr->ContextRecord->Rip = reinterpret_cast<DWORD64>(&mySegFaultHandler);
        SetThreadContext(GetCurrentThread(), ptr->ContextRecord);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

bool setBadMemoryAccessHandler()
{
    return AddVectoredExceptionHandler(1, &accessViolationHandler) != NULL;
}
}

#endif