#include "ChannelInfo.hpp"

namespace YADAW::Entity
{
ChannelInfo::ChannelInfo(QObject* parent): QObject(parent), name_(), color_()
{}

ChannelInfo::ChannelInfo(const ChannelInfo& rhs):
    QObject(rhs.parent()), name_(rhs.name_), color_(rhs.color_)
{}

ChannelInfo::~ChannelInfo()
{}

const QString& ChannelInfo::name() const
{
    return name_;
}

QColor ChannelInfo::color() const
{
    return color_;
}

void ChannelInfo::setName(const QString& name)
{
    name_ = name;
    nameChanged();
}

void ChannelInfo::setColor(QColor color)
{
    color_ = color;
    colorChanged();
}
}