#include "TimeSignatureSequence.hpp"

#include <algorithm>

namespace YADAW::Audio::Base
{
TimeSignatureSequence::TimeSignatureSequence(std::uint8_t numerator, std::uint8_t denominator):
    container_(1, TimeSignatureWithTime(numerator, denominator))
{}

std::size_t TimeSignatureSequence::size() const
{
    return container_.size();
}

TimeSignatureSequence::TimeSignatureWithTime TimeSignatureSequence::operator[](std::size_t index) const
{
    return container_[index];
}

std::optional<TimeSignatureSequence::TimeSignatureWithTime>
    TimeSignatureSequence::at(std::size_t index) const
{
    return index < size()?
        std::optional(container_[index]):
        std::nullopt;
}

TimeSignature TimeSignatureSequence::operator()(std::uint16_t bar) const
{
    if(bar == 0)
    {
        return container_.front().timeSignature();
    }
    auto index = lowerBound(bar);
    if(index == size())
    {
        return container_.back().timeSignature();
    }
    if(auto element = container_[index]; element.bar() == bar)
    {
        return element.timeSignature();
    }
    return container_[index - 1].timeSignature();
}

std::size_t TimeSignatureSequence::lowerBound(std::uint16_t bar) const
{
    return std::distance(container_.begin(),
        std::lower_bound(container_.begin(), container_.end(), bar,
            [](const TimeSignatureWithTime& element, std::uint16_t value)
            {
                return element.bar() < value;
            }
        )
    );
}

std::size_t TimeSignatureSequence::upperBound(std::uint16_t bar) const
{
    return std::distance(container_.begin(),
        std::upper_bound(container_.begin(), container_.end(), bar,
            [](std::uint16_t value, const TimeSignatureWithTime& element)
            {
                return value < element.bar();
            }
        )
    );
}
}