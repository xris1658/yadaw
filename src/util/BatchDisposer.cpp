#include "BatchDisposer.hpp"

namespace YADAW::Util
{
BatchDisposer::BatchDisposer()
{}

BatchDisposer::~BatchDisposer()
{}

void BatchDisposer::addObject(BatchDisposer::Object&& obj)
{
    disposingObjects_.emplace_back(std::move(obj));
}
}
