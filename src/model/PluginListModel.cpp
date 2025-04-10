#include "PluginListModel.hpp"

#include "util/QmlUtil.hpp"

#include <thread>

namespace YADAW::Model
{
namespace Impl
{
YADAW::Model::IPluginListModel::PluginFormat getPluginFormat(int format)
{
    switch(format)
    {
    case YADAW::DAO::PluginFormat::PluginFormatVST3:
        return YADAW::Model::IPluginListModel::PluginFormat::VST3;
    case YADAW::DAO::PluginFormat::PluginFormatCLAP:
        return YADAW::Model::IPluginListModel::PluginFormat::CLAP;
    case YADAW::DAO::PluginFormat::PluginFormatVestifal:
        return YADAW::Model::IPluginListModel::PluginFormat::Vestifal;
    default:
        return YADAW::Model::IPluginListModel::PluginFormat::UnknownFormat;
    }
}

YADAW::Model::IPluginListModel::PluginType getPluginType(int type)
{
    switch(type)
    {
    case YADAW::DAO::PluginType::PluginTypeMIDIEffect:
        return YADAW::Model::IPluginListModel::PluginType::MIDIEffect;
    case YADAW::DAO::PluginType::PluginTypeInstrument:
        return YADAW::Model::IPluginListModel::PluginType::Instrument;
    case YADAW::DAO::PluginType::PluginTypeAudioEffect:
        return YADAW::Model::IPluginListModel::PluginType::AudioEffect;
    default:
        return YADAW::Model::IPluginListModel::PluginType::UnknownType;
    }
}

template<std::size_t I>
const auto& get(const YADAW::DAO::PluginInfoInDatabase& info)
{
    auto tie = info.createConstTie();
    return std::get<I>(tie);
}

template<std::size_t I>
auto& get(YADAW::DAO::PluginInfoInDatabase& info)
{
    auto tie = info.createTie();
    return std::get<I>(tie);
}

template<std::size_t I>
bool isLess(const YADAW::DAO::PluginInfoInDatabase& lhs, const YADAW::DAO::PluginInfoInDatabase& rhs)
{
    return Impl::get<I>(lhs) < Impl::get<I>(rhs);
}

template<std::size_t I>
bool isPassed(const YADAW::DAO::PluginInfoInDatabase& info, const QString& string, Qt::CaseSensitivity caseSensitivity)
{
    return false;
}

template<>
bool isPassed<1>(const YADAW::DAO::PluginInfoInDatabase& info, const QString& string, Qt::CaseSensitivity caseSensitivity)
{
    return info.path.contains(string, caseSensitivity);
}

template<>
bool isPassed<3>(const YADAW::DAO::PluginInfoInDatabase& info, const QString& string, Qt::CaseSensitivity caseSensitivity)
{
    return info.name.contains(string, caseSensitivity);
}

template<>
bool isPassed<4>(const YADAW::DAO::PluginInfoInDatabase& info, const QString& string, Qt::CaseSensitivity caseSensitivity)
{
    return info.vendor.contains(string, caseSensitivity);
}

template<>
bool isPassed<5>(const YADAW::DAO::PluginInfoInDatabase& info, const QString& string, Qt::CaseSensitivity caseSensitivity)
{
    return info.version.contains(string, caseSensitivity);
}

constexpr decltype(&isLess<0>) isLessFuncs[] = {
    &isLess<0>,
    &isLess<1>,
    &isLess<2>,
    &isLess<3>,
    &isLess<4>,
    &isLess<5>,
    &isLess<6>,
    &isLess<7>,
};

constexpr decltype(&isPassed<0>) isPassedFuncs[] = {
    &isPassed<0>,
    &isPassed<1>,
    &isPassed<2>,
    &isPassed<3>,
    &isPassed<4>,
    &isPassed<5>,
    &isPassed<6>,
    &isPassed<7>,
};
}

PluginListModel::PluginListModel(const std::function<List()>& updateListFunc, QObject* parent):
    IPluginListModel(parent),
    updateListFunc_(updateListFunc)
{
    YADAW::Util::setCppOwnership(*this);
    update();
}

PluginListModel::PluginListModel(std::function<List()>&& updateListFunc, QObject* parent):
    IPluginListModel(parent),
    updateListFunc_(std::move(updateListFunc))
{
    YADAW::Util::setCppOwnership(*this);
    update();
}

PluginListModel::~PluginListModel()
{
}

int PluginListModel::itemCount() const
{
    return data_.size();
}

int PluginListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant PluginListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& data = data_[row];
        // Note that Role::Uid is not included since we don't need to "show" or "use" the plugin UID
        switch(role)
        {
        case Role::Id:
            return QVariant::fromValue(data.id);
        case Role::Path:
            return QVariant::fromValue(data.path);
        case Role::Name:
            return QVariant::fromValue(data.name);
        case Role::Vendor:
            return QVariant::fromValue(data.vendor);
        case Role::Version:
            return QVariant::fromValue(data.version);
        case Role::Format:
            return QVariant::fromValue<int>(Impl::getPluginFormat(data.format));
        case Role::Type:
            return QVariant::fromValue<int>(Impl::getPluginType(data.type));
        default:
            return {};
        }
    }
    return {};
}

bool PluginListModel::isComparable(int role) const
{
    if(role >= Qt::UserRole && role < RoleCount)
    {
        return true;
    }
    return false;
}

bool PluginListModel::isFilterable(int roleIndex) const
{
    return roleIndex == Role::Format
        || roleIndex == Role::Type;
}

bool PluginListModel::isSearchable(int roleIndex) const
{
    return roleIndex == Role::Path
        || roleIndex == Role::Name
        || roleIndex == Role::Vendor
        || roleIndex == Role::Version;
}

bool PluginListModel::isLess(int role, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if(role >= Qt::UserRole && role < RoleCount)
    {
        if(lhs.model() == this && rhs.model() == this)
        {
            return Impl::isLessFuncs[role - Qt::UserRole](data_[lhs.row()], data_[rhs.row()]);
        }
    }
    return false;
}

bool PluginListModel::isSearchPassed(int role, const QModelIndex& modelIndex,
    const QString& string, Qt::CaseSensitivity caseSensitivity) const
{
    if(role >= Qt::UserRole && role < RoleCount)
    {
        if(modelIndex.model() == this)
        {
            return Impl::isPassedFuncs[role - Qt::UserRole](
                data_[modelIndex.row()], string, caseSensitivity);
        }
    }
    return false;
}

bool PluginListModel::isPassed(const QModelIndex& modelIndex, int role,
    const QVariant& variant) const
{
    auto row = modelIndex.row();
    if(modelIndex.model() == this && row >= 0 && row < itemCount())
    {
        const auto& data = data_[row];
        switch(role)
        {
        case Role::Format:
        {
            return Impl::getPluginFormat(data.format) == variant.value<int>();
        }
        case Role::Type:
        {
            return Impl::getPluginType(data.type) == variant.value<int>();
        }
        }
    }
    return false;
}

void PluginListModel::clear()
{
    if(!data_.empty())
    {
        beginRemoveRows(QModelIndex(), 0, data_.size() - 1);
        data_.clear();
        endRemoveRows();
    }
}

void PluginListModel::update()
{
    auto list = updateListFunc_();
    clear();
    if(!list.empty())
    {
        beginInsertRows(QModelIndex(), 0, list.size() - 1);
        data_ = std::move(list);
        endInsertRows();
    }
}
}
