#ifndef YADAW_SRC_CONTROLLER_LIBRARYPLUGINMAP
#define YADAW_SRC_CONTROLLER_LIBRARYPLUGINMAP

#include "audio/plugin/IAudioPlugin.hpp"
#include "native/Library.hpp"

#include <map>
#include <set>

namespace YADAW::Controller
{
namespace Impl
{
using YADAW::Native::Library;
struct CompareLibrary
{
    using is_transparent = void;
    bool operator()(const Library& lhs, const Library& rhs) const
    {
        return lhs.path() < rhs.path();
    }
    bool operator()(const Library& lhs, const QString& rhs) const
    {
        return lhs.path() < rhs;
    }
    bool operator()(const QString& lhs, const Library& rhs) const
    {
        return lhs < rhs.path();
    }
};

struct ComparePlugin
{
    using is_transparent = void;
    bool operator()(
        const std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>& lhs,
        const std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>& rhs) const
    {
        return lhs < rhs;
    }
    bool operator()(
        const std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>& lhs,
        const YADAW::Audio::Plugin::IAudioPlugin* rhs) const
    {
        return lhs.get() < rhs;
    }
    bool operator()(
        const YADAW::Audio::Plugin::IAudioPlugin* lhs,
        const std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>& rhs) const
    {
        return lhs < rhs.get();
    }
};
}

using LibraryPluginMap = std::map<
    YADAW::Native::Library,
    std::set<
        std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>,
        Impl::ComparePlugin
    >,
    Impl::CompareLibrary
>;

LibraryPluginMap& appLibraryPluginMap();
}

#endif // YADAW_SRC_CONTROLLER_LIBRARYPLUGINMAP
