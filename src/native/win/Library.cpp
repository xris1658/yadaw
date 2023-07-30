#if(WIN32)

#include "native/Library.hpp"

#include <libloaderapi.h>

namespace YADAW::Native
{
Library::Library(): handle_(NULL)
{
}

Library::Library(const QString& path): path_(path)
{
    auto widePath = path_.toStdWString();
    handle_ =
        LoadLibraryExW(widePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if(handle_ == NULL)
    {
        errorCode_ = GetLastError();
    }
}

Library::Library(Library&& rhs) noexcept: handle_(NULL)
{
    std::swap(path_, rhs.path_);
    std::swap(handle_, rhs.handle_);
}

Library& Library::operator=(Library&& rhs) noexcept
{
    if(handle_ != rhs.handle_)
    {
        std::swap(path_, rhs.path_);
        std::swap(handle_, rhs.handle_);
    }
    return *this;
}

Library::~Library() noexcept
{
    if(handle_)
    {
        path_.clear();
        FreeLibrary(handle_);
    }
}

bool Library::loaded() const
{
    return handle_ != NULL;
}

const QString& Library::path() const
{
    return path_;
}

YADAW::Native::Library::ExportType Library::getExportImpl(const char* name) const
{
    return GetProcAddress(reinterpret_cast<HMODULE>(handle_), name);
}

void* Library::handle() const
{
    return reinterpret_cast<void*>(handle_);
}
}

#endif