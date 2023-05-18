#ifndef YADAW_NATIVE_LIBRARY
#define YADAW_NATIVE_LIBRARY

#include "native/Native.hpp"

#include <QString>

#include <memory>

namespace YADAW::Native
{
class LibraryImpl;

class Library
{
public:
#if(WIN32)
    using ExportType = FARPROC;
#elif(__linux__)
    using ExportType = void* const;
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
    QString path() const;
    template<typename T>
    T getExport(const char* name) const
    {
        return (T)getExportImpl(name);
    }
public:
    std::unique_ptr<LibraryImpl> pImpl_;
};
}

#endif //YADAW_NATIVE_LIBRARY
