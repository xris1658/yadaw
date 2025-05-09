#include "LibraryPluginPool.hpp"

namespace YADAW::Controller
{
LibraryPluginPool::Instance::Instance():
    pool_(nullptr)
{}

LibraryPluginPool::Instance::Instance(
    LibraryPluginPool& pool,
    LibraryPluginPool::Pool::iterator libraryIt,
    LibraryPluginPool::Pool::value_type::second_type::iterator pluginIt):
    pool_(&pool)
{
    new(YADAW::Util::AlignHelper<LibraryIterator>::fromAligned(&libraryIt_)) LibraryIterator(libraryIt);
    new(YADAW::Util::AlignHelper<PluginIterator>::fromAligned(&pluginIt_)) PluginIterator(pluginIt);
}

LibraryPluginPool::Instance::Instance(Instance&& rhs):
    pool_(rhs.pool_)
{
    new(YADAW::Util::AlignHelper<LibraryIterator>::fromAligned(&libraryIt_)) LibraryIterator(
        *YADAW::Util::AlignHelper<LibraryIterator>::fromAligned(&rhs.libraryIt_)
    );
    new(YADAW::Util::AlignHelper<PluginIterator>::fromAligned(&pluginIt_)) PluginIterator(
        *YADAW::Util::AlignHelper<PluginIterator>::fromAligned(&rhs.pluginIt_)
    );
    pool_ = std::exchange(rhs.pool_, nullptr);
    std::destroy_at(YADAW::Util::AlignHelper<LibraryIterator>::fromAligned(&rhs.libraryIt_));
    std::destroy_at(YADAW::Util::AlignHelper<PluginIterator>::fromAligned(&rhs.pluginIt_));
}

LibraryPluginPool::Instance& LibraryPluginPool::Instance::operator=(Instance&& rhs)
{
    std::destroy_at(this);
    new(this) Instance(std::move(rhs));
}

LibraryPluginPool::Instance::~Instance()
{
    if(pool_)
    {
        auto libraryIt = YADAW::Util::AlignHelper<LibraryIterator>::fromAligned(&libraryIt_);
        auto pluginIt = YADAW::Util::AlignHelper<PluginIterator>::fromAligned(&pluginIt_);
        (*libraryIt)->second.erase(*pluginIt);
        if((*libraryIt)->second.empty() && pool_->unloadLibraryIfNoPlugins())
        {
            pool_->pool_.erase(*libraryIt);
        }
        std::destroy_at(libraryIt);
        std::destroy_at(pluginIt);
    }
}

LibraryPluginPool::Instance::operator bool() const
{
    return pool_;
}

OptionalRef<const YADAW::Native::Library> LibraryPluginPool::Instance::library() const
{
    if(pool_)
    {
        return {(*YADAW::Util::AlignHelper<LibraryIterator>::fromAligned(&libraryIt_))->first};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Plugin::IAudioPlugin> LibraryPluginPool::Instance::plugin() const
{
    if(pool_)
    {
        return {***YADAW::Util::AlignHelper<PluginIterator>::fromAligned(&pluginIt_)};
    }
    return std::nullopt;
}

bool LibraryPluginPool::unloadLibraryIfNoPlugins() const
{
    return unloadLibraryIfNoPlugins_;
}

void LibraryPluginPool::setUnloadLibraryIfNoPlugins(bool unloadLibraryIfNoPlugins)
{
    if(unloadLibraryIfNoPlugins ^ unloadLibraryIfNoPlugins_)
    {
        unloadLibraryIfNoPlugins_ = unloadLibraryIfNoPlugins;
        if(unloadLibraryIfNoPlugins)
        {
            std::erase_if(
                pool_,
                [](Pool::const_reference pair)
                {
                    return pair.second.empty();
                }
            );
        }
    }
}

LibraryPluginPool::Instance LibraryPluginPool::createPluginInstance(
    const QString& path, std::function<CreatePluginFromLibraryCallback>& callback)
{
    auto it = pool_.lower_bound(path);
    if(it == pool_.end() || it->first.path() != path)
    {
        if(YADAW::Native::Library library(path); library.loaded())
        {
            it = pool_.emplace_hint(
                it,
                std::make_pair(std::move(library), Pool::value_type::second_type())
            );
        }
        else
        {
            return {};
        }
    }
    if(auto plugin = callback(it->first))
    {
        return {*this, it, it->second.emplace(std::move(plugin)).first};
    }
    if(it->second.empty() && unloadLibraryIfNoPlugins_)
    {
        pool_.erase(it);
    }
    return {};
}
}
