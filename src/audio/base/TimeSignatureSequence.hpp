#ifndef YADAW_SRC_AUDIO_BASE_TIMESIGNATURESEQUENCE
#define YADAW_SRC_AUDIO_BASE_TIMESIGNATURESEQUENCE

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace YADAW::Audio::Base
{
struct TimeSignature
{
    std::uint8_t numerator;
    std::uint8_t denominator;
    bool operator==(TimeSignature rhs) const
    {
        return numerator == rhs.numerator && denominator == rhs.denominator;
    }
    bool operator!=(TimeSignature rhs) const
    {
        return !operator==(rhs);
    }
};

class TimeSignatureSequence
{
public:
    class TimeSignatureWithTime
    {
    public:
        TimeSignatureWithTime(std::uint8_t numerator, std::uint8_t denominator,
            std::uint16_t bar = 0):
            timeSignature_{numerator, denominator}, bar_(bar) {}
    public:
        TimeSignature timeSignature() const { return timeSignature_; }
        std::uint16_t bar() const { return bar_; }
        void setTimeSignature(TimeSignature timeSignature) { timeSignature_ = timeSignature; }
        void setBar(std::uint16_t bar) { bar_ = bar; }
    private:
        TimeSignature timeSignature_;
        std::uint16_t bar_;
    };
public:
    TimeSignatureSequence(std::uint8_t numerator, std::uint8_t denominator);
public:
    std::size_t size() const;
    TimeSignatureWithTime operator[](std::size_t index) const;
    std::optional<TimeSignatureWithTime> at(std::size_t index) const;
    TimeSignature operator()(std::uint16_t bar) const;
    std::size_t lowerBound(std::uint16_t bar) const;
    std::size_t upperBound(std::uint16_t bar) const;
private:
    std::vector<TimeSignatureWithTime> container_;
};
}

#endif //YADAW_SRC_AUDIO_BASE_TIMESIGNATURESEQUENCE
