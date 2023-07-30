#if(__linux__)

#include "native/Library.hpp"

#include <dlfcn.h>
#include <errno.h>

namespace YADAW::Native
{
Library::Library(): handle_(nullptr)
{
}

Library::Library(const QString& path): path_(path)
{
    auto pathAsString = path.toStdString();
    handle_ = reinterpret_cast<decltype(handle_)>(dlopen(pathAsString.c_str(), RTLD_NOW));
    if(handle_ == nullptr)
    {
        qDebug("%s", dlerror());
    }
}

Library::Library(Library&& rhs) noexcept: handle_(nullptr)
{
    std::swap(path_, rhs.path_);
    std::swap(handle_, rhs.handle_);
}

Library& Library::operator=(YADAW::Native::Library&& rhs) noexcept
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
        dlclose(handle_);
    }
}

bool Library::loaded() const
{
    return handle_ != nullptr;
}

const QString& Library::path() const
{
    return path_;
}

YADAW::Native::Library::ExportType Library::getExportImpl(const char* name) const
{
    return dlsym(handle_, name);
}

void* Library::handle() const
{
    return reinterpret_cast<void*>(handle_);
}
}

#endif