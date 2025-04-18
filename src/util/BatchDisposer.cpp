#include "BatchDisposer.hpp"

namespace YADAW::Util
{
BatchDisposer::BatchDisposer(std::function<PreDisposeCallback>&& callback):
    preDisposingObjectCallback_(std::move(callback))
{}

BatchDisposer::~BatchDisposer()
{
    preDisposingObjectCallback_();
}

void BatchDisposer::doAddObject(BatchDisposer::Object&& obj)
{
    disposingObjects_.emplace_back(std::move(obj));
}
}
