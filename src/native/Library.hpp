#ifndef YADAW_SRC_NATIVE_LIBRARY
#define YADAW_SRC_NATIVE_LIBRARY

#include "native/Native.hpp"

#include <QString>

#include <memory>

namespace YADAW::Native
{
class Library
{
private:
#if(WIN32)
    using HandleType = HMODULE;
#elif(__linux__)
    using HandleType = void*;
#else
#error Unknown HandleType
#endif
public:
#if(WIN32)
    using ExportType = FARPROC;
#elif(__linux__)
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
#if(WIN32)
    ErrorCodeType errorCode_ = 0;
#endif
    HandleType handle_;
};
}

#endif // YADAW_SRC_NATIVE_LIBRARY
