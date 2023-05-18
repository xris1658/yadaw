#include "Library.hpp"

#include "native/LibraryImpl.hpp"

namespace YADAW::Native
{
Library::Library(): pImpl_()
{
}

Library::Library(const QString& path):
    pImpl_(std::make_unique<YADAW::Native::LibraryImpl>(path))
{
}

Library::Library(Library&& rhs) noexcept: pImpl_(std::move(rhs.pImpl_))
{

}

Library& Library::operator=(Library&& rhs) noexcept
{
    pImpl_ = std::move(rhs.pImpl_);
    return *this;
}

Library::~Library() noexcept
{
}

bool Library::loaded() const
{
    return pImpl_? pImpl_->loaded(): false;
}

QString Library::path() const
{
    return pImpl_? pImpl_->path(): QString();
}

Library::ExportType Library::getExportImpl(const char* name) const
{
    return pImpl_? pImpl_->getExport(name): nullptr;
}
}