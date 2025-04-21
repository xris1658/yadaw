#include "BatchUpdater.hpp"

namespace YADAW::Util
{
BatchUpdater::BatchUpdater(std::function<UpdateCallback>&& callback):
    updateCallback_(std::move(callback))
{}

BatchUpdater::~BatchUpdater()
{
    if(disposingObjects_.empty() || hasNull_)
    {
        updateCallback_();
    }
}

void BatchUpdater::addNull()
{
    hasNull_ = true;
}

void BatchUpdater::commit()
{
    if(disposingObjects_.empty() || hasNull_)
    {
        updateCallback_();
        disposingObjects_.clear();
        hasNull_ = false;
    }
}

void BatchUpdater::doAddObject(BatchUpdater::Object&& obj)
{
    disposingObjects_.emplace_back(std::move(obj));
}
}
