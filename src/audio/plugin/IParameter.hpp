#ifndef YADAW_SRC_AUDIO_PLUGIN_IPARAMETER
#define YADAW_SRC_AUDIO_PLUGIN_IPARAMETER

#include <QString>

#include <cstdint>
#include <type_traits>

namespace YADAW::Audio::Plugin
{
enum ParameterFlags: std::int32_t
{
    Discrete            = 1 << 0,
    Periodic            = 1 << 1,
    Hidden              = 1 << 2,
    Readonly            = 1 << 3,
    Automatable         = 1 << 4,
    SupportMinMaxValue  = 1 << 5,
    SupportDefaultValue = 1 << 6,
    ShowAsList          = 1 << 7,
    ShowAsSwitch        = 1 << 8
};

class IParameter
{
public:
    virtual ~IParameter() = default;
public:
    virtual std::uint32_t id() const = 0;
    virtual QString name() const = 0;
    virtual double minValue() const = 0;
    virtual double maxValue() const = 0;
    virtual double defaultValue() const = 0;
    virtual double value() const = 0;
    virtual void setValue(double value) = 0;
    virtual double stepSize() const = 0;
    virtual std::uint32_t stepCount() const = 0;
    virtual QString unit() const = 0;
public:
    virtual QString valueToString(double value) const = 0;
    virtual double stringToValue(const QString& string) const = 0;
public:
    std::int32_t flags() const
    {
        return flags_;
    }
protected:
    std::int32_t flags_ = 0;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_IPARAMETER
