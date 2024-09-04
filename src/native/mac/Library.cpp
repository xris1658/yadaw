#if __APPLE__

#include "native/Library.hpp"

#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBundle.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFString.h>

namespace YADAW::Native
{
Library::Library(): handle_(nullptr)
{}

Library::Library(const QString& path): path_(path)
{
    auto pathAsCFString = path.toCFString();
    auto url = CFURLCreateWithFileSystemPath(
        kCFAllocatorDefault,pathAsCFString, kCFURLPOSIXPathStyle, true
    );
    handle_ = CFBundleCreate(kCFAllocatorDefault, url);
    auto loadResult = CFBundleLoadExecutable(handle_);
    if(!loadResult)
    {
        CFRelease(handle_);
        handle_ = nullptr;
    }
    CFRelease(url);
    CFRelease(pathAsCFString);
}

Library::Library(Library&& rhs) noexcept: handle_(nullptr)
{
    std::swap(path_, rhs.path_);
    std::swap(handle_, rhs.handle_);
    std::swap(error_, rhs.error_);
}

Library& Library::operator=(Library&& rhs) noexcept
{
    if(handle_ != rhs.handle_)
    {
        std::swap(path_, rhs.path_);
        std::swap(handle_, rhs.handle_);
        std::swap(error_, rhs.error_);
    }
    return *this;
}

Library::~Library() noexcept
{
    if(handle_)
    {
        CFBundleUnloadExecutable(handle_);
        CFRelease(handle_);
    }
}

Library::ExportType Library::getExportImpl(const char *name) const
{
    auto string = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    auto ret = CFBundleGetFunctionPointerForName(handle_, string);
    CFRelease(string);
    return ret;
}

bool Library::loaded() const
{
    return handle_;
}

const QString& Library::path() const
{
    return path_;
}

void* Library::handle() const
{
    return reinterpret_cast<void*>(handle_);
}
}

#endif