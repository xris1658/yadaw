#include "native/LibraryImpl.hpp"

#include <dlfcn.h>
#include <errno.h>

namespace YADAW::Native
{
LibraryImpl::LibraryImpl(): handle_(nullptr)
{
}

LibraryImpl::LibraryImpl(const QString& path): path_(path)
{
    auto pathAsString = path.toStdString();
    handle_ = reinterpret_cast<decltype(handle_)>(dlopen(pathAsString.c_str(), 0));
    if(handle_ == nullptr)
    {
        dlerror();
    }
}

LibraryImpl::LibraryImpl(LibraryImpl&& rhs) noexcept: handle_(nullptr)
{
    std::swap(path_, rhs.path_);
    std::swap(handle_, rhs.handle_);
}

LibraryImpl& LibraryImpl::operator=(YADAW::Native::LibraryImpl&& rhs) noexcept
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
        dlclose(handle_);
    }
}

bool LibraryImpl::loaded() const
{
    return handle_ != nullptr;
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
    return dlsym(handle_, name);
}
}