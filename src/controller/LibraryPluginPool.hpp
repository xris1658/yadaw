#ifndef YADAW_SRC_CONTROLLER_LIBRARYPLUGINPOOL
#define YADAW_SRC_CONTROLLER_LIBRARYPLUGINPOOL

#include "audio/plugin/IAudioPlugin.hpp"
#include "controller/LibraryPluginMap.hpp"
#include "native/Library.hpp"
#include "util/AlignHelper.hpp"

#include <map>
#include <memory>
#include <set>

namespace YADAW::Controller
{
class LibraryPluginPool
{
    using Pool = std::map<
        YADAW::Native::Library,
        std::set<
            std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>,
            Impl::ComparePlugin
        >,
        Impl::CompareLibrary
    >;
    using LibraryIterator = Pool::iterator;
    using PluginIterator = Pool::value_type::second_type::iterator;
    friend class Instance;
public:
    using CreatePluginFromLibraryCallback =
    std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>(
        const YADAW::Native::Library& library
    );
public:
    class Instance
    {
        friend class LibraryPluginPool;
    public:
        Instance();
    private:
        Instance(
            LibraryPluginPool& pool,
            LibraryPluginPool::LibraryIterator libraryIt,
            LibraryPluginPool::PluginIterator pluginIt
        );
    public:
        Instance(Instance&& rhs);
        Instance& operator=(Instance&& rhs);
        ~Instance();
    public:
        operator bool() const;
        OptionalRef<const YADAW::Native::Library> library() const;
        OptionalRef<YADAW::Audio::Plugin::IAudioPlugin> plugin() const;
    private:
        LibraryPluginPool* pool_;
        YADAW::Util::AlignedStorage<LibraryPluginPool::LibraryIterator> libraryIt_;
        YADAW::Util::AlignedStorage<LibraryPluginPool::PluginIterator> pluginIt_;
    };
public:
    bool unloadLibraryIfNoPlugins() const;
    void setUnloadLibraryIfNoPlugins(bool unloadLibraryIfNoPlugins);
    Instance createPluginInstance(
        const QString& path,
        const std::function<CreatePluginFromLibraryCallback>& callback
    );
private:
    Pool pool_;
    bool unloadLibraryIfNoPlugins_;
};

LibraryPluginPool& appLibraryPluginPool();
}

#endif // YADAW_SRC_CONTROLLER_LIBRARYPLUGINPOOL