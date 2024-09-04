#ifndef YADAW_SRC_NATIVE_LIBRARY
#define YADAW_SRC_NATIVE_LIBRARY

#include "native/Native.hpp"

#include <QString>

#if _WIN32
#elif __APPLE__
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBundle.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFError.h>
#endif

#include <memory>

namespace YADAW::Native
{
class Library
{
private:
#if _WIN32
    using HandleType = HMODULE;
#elif __APPLE__
    using HandleType = CFBundleRef;
#elif __linux__
    using HandleType = void*;
#else
#error Unknown HandleType
#endif
public:
#if _WIN32
    using ExportType = FARPROC;
#elif __APPLE__
    using ExportType = void*;
#elif __linux__
    using ExportType = void*;
#else
#error Unknown ExportType
#endif
public:
    Library();
    explicit Library(const QString& path);
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;
    Library(Library&& rhs) noexcept;
    Library& operator=(Library&& rhs) noexcept;
    ~Library() noexcept;
private:
    ExportType getExportImpl(const char* name) const;
public:
    bool loaded() const;
    const QString& path() const;
    template<typename T>
    T getExport(const char* name) const
    {
        return (T)getExportImpl(name);
    }
    void* handle() const;
private:
    QString path_;
#if _WIN32
    ErrorCodeType errorCode_ = 0;
#elif __APPLE__
    CFErrorRef error_;
#endif
    HandleType handle_;
};
}

#endif // YADAW_SRC_NATIVE_LIBRARY
