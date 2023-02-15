#include "LibraryImpl.hpp"

#include <libloaderapi.h>

namespace YADAW::Native
{
LibraryImpl::LibraryImpl(): handle_(NULL)
{
}

LibraryImpl::LibraryImpl(const QString& path): path_(path)
{
    auto widePath = path_.toStdWString();
    handle_ = reinterpret_cast<decltype(handle_)>(
        LoadLibraryExW(widePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH));
    if(handle_ == NULL)
    {
        errorCode_ = GetLastError();
        handle_ = NULL;
    }
}

LibraryImpl::LibraryImpl(LibraryImpl&& rhs) noexcept: handle_(NULL)
{
    std::swap(path_, rhs.path_);
    std::swap(handle_, rhs.handle_);
}

LibraryImpl& LibraryImpl::operator=(LibraryImpl&& rhs) noexcept
{
    if(handle_ != rhs.handle_)
    {
        std::swap(path_, rhs.path_);
        std::swap(handle_, rhs.handle_);
    }
    return *this;
}

LibraryImpl::~LibraryImpl() noexcept
{
    if(handle_)
    {
        path_.clear();
        FreeLibrary(reinterpret_cast<HMODULE>(handle_));
    }
}

ErrorCodeType LibraryImpl::errorCode() const
{
    return errorCode_;
}

const QString& LibraryImpl::path() const
{
    return path_;
}

YADAW::Native::Library::ExportType LibraryImpl::getExport(const char* name) const
{
    return GetProcAddress(reinterpret_cast<HMODULE>(handle_), name);
}
}