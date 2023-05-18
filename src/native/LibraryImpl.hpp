#ifndef YADAW_NATIVE_LIBRARYIMPL
#define YADAW_NATIVE_LIBRARYIMPL

#include "native/Library.hpp"
#include "native/Native.hpp"

#include <QString>

namespace YADAW::Native
{
class LibraryImpl
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
    LibraryImpl();
    explicit LibraryImpl(const QString& path);
    LibraryImpl(const LibraryImpl&) = delete;
    LibraryImpl& operator=(const LibraryImpl&) = delete;
    LibraryImpl(LibraryImpl&& rhs) noexcept;
    LibraryImpl& operator=(LibraryImpl&& rhs) noexcept;
    ~LibraryImpl() noexcept;
public:
    bool loaded() const;
    ErrorCodeType errorCode() const;
    const QString& path() const;
    YADAW::Native::Library::ExportType getExport(const char* name) const;
private:
    QString path_;
    ErrorCodeType errorCode_ = 0;
    HandleType handle_;
};
}

#endif //YADAW_NATIVE_LIBRARYIMPL
