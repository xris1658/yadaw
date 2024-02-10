#ifndef YADAW_SRC_CONTROLLER_PLUGINPOOLCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINPOOLCONTROLLER

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
}

using PluginPool = std::map<
    YADAW::Native::Library,
    std::set<std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>>,
    Impl::CompareLibrary
>;

PluginPool& appPluginPool();
}

#endif // YADAW_SRC_CONTROLLER_PLUGINPOOLCONTROLLER
