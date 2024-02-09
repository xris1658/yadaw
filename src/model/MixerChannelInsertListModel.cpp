#include "MixerChannelInsertListModel.hpp"

#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/engine/MultiInputDeviceWithPDC.hpp"
#include "controller/PluginListController.hpp"
#include "controller/PluginPoolController.hpp"
#include "dao/PluginTable.hpp"

namespace YADAW::Model

using YADAW::Audio::Engine::MultiInputDeviceWithPDC;
using YADAW::Audio::Plugin::IAudioPlugin;
{
MixerChannelInsertListModel::MixerChannelInsertListModel(
    YADAW::Audio::Mixer::Inserts& inserts, QObject* parent):
    IMixerChannelInsertListModel(parent),
    inserts_(&inserts)
{}

MixerChannelInsertListModel::~MixerChannelInsertListModel()
{}

int MixerChannelInsertListModel::itemCount() const
{
    return inserts_->insertCount();
}

int MixerChannelInsertListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant MixerChannelInsertListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& graph = inserts_->graph();
        const auto& node = *(inserts_->insertAt(row));
        const auto& nodeData = graph.getNodeData(node);
        auto device = nodeData.process.device();
        if(device->audioInputGroupCount() > 1)
        {
            auto multiInput = dynamic_cast<const MultiInputDeviceWithPDC*>(device);
            if(multiInput)
            {
                device = multiInput->process().device();
            }
        }
        switch(role)
        {
        case Role::Name:
        {
            return QVariant::fromValue<QString>(*inserts_->insertNameAt(row));
        }
        case Role::Bypassed:
        {
            return QVariant::fromValue<bool>(*inserts_->insertBypassed(row));
        }
        case Role::HasUI:
        {
            auto ret = false;
            auto plugin = dynamic_cast<const IAudioPlugin*>(device);
            if(plugin)
            {
                auto gui = plugin->gui();
                ret = gui;
            }
            return QVariant::fromValue<bool>(ret);
        }
        case Role::WindowVisible:
        {
            auto ret = false;
            auto plugin = dynamic_cast<const IAudioPlugin*>(device);
            if(plugin)
            {
                auto gui = plugin->gui();
                if(gui)
                {
                    auto window = gui->window();
                    if(window)
                    {
                        ret = window->isVisible();
                    }
                }
            }
            return QVariant::fromValue<bool>(ret);
        }
        }
    }
    return {};
}

bool MixerChannelInsertListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& graph = inserts_->graph();
        const auto& node = *(inserts_->insertAt(row));
        const auto& nodeData = graph.getNodeData(node);
        auto device = nodeData.process.device();
        if(device->audioInputGroupCount() > 1)
        {
            auto multiInput = dynamic_cast<const MultiInputDeviceWithPDC*>(device);
            if(multiInput)
            {
                device = multiInput->process().device();
            }
        }
        switch(role)
        {
        case Role::Bypassed:
        {
            inserts_->setBypassed(row, value.value<bool>());
            dataChanged(index, index, {Role::Bypassed});
            return true;
        }
        case Role::WindowVisible:
        {
            auto plugin = dynamic_cast<const IAudioPlugin*>(device);
            if(plugin)
            {
                auto gui = plugin->gui();
                if(gui)
                {
                    auto window = gui->window();
                    if(window)
                    {
                        auto visible = value.value<bool>();
                        if(window->isVisible() != visible)
                        {
                            window->setVisible(visible);
                            dataChanged(index, index, {Role::WindowVisible});
                        }
                    }
                }
            }
        }
        }
    }
    return false;
}

bool YADAW::Model::MixerChannelInsertListModel::insert(int position, int pluginId)
{
    if(position >= 0 && position <= inserts_->insertCount())
    {
        auto& pool = YADAW::Controller::appPluginPool();
        const auto& pluginListModel = YADAW::Controller::appPluginListModel();
        auto pluginInfo = YADAW::DAO::selectPluginById(pluginId);
        auto it = pool.find(pluginInfo.path);
        if(it == pool.end())
        {
            auto library = YADAW::Native::Library(pool);
            //
        }
    }
}

bool YADAW::Model::MixerChannelInsertListModel::append(int pluginId)
{
    insert(inserts_->insertCount(), pluginId);
}

bool MixerChannelInsertListModel::remove(int position, int removeCount)
{
    auto ret = inserts_->remove(position, removeCount);
    if(ret)
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        endRemoveRows();
    }
    return ret;
}

void MixerChannelInsertListModel::clear()
{
    remove(0, inserts_->insertCount());
}
}