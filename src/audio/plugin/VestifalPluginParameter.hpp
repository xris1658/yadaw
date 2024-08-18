#ifndef YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGINPARAMETER
#define YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGINPARAMETER

#include "audio/device/IAudioDeviceParameter.hpp"

#include "audio/plugin/vestifal/Vestifal.h"

#include <vector>

namespace YADAW::Audio::Plugin
{
class VestifalParameter: public YADAW::Audio::Device::IParameter
{
public:
    VestifalParameter(AEffect* effect, std::uint32_t index);
public:
    std::uint32_t id() const override;
    QString name() const override;
    double minValue() const override;
    double maxValue() const override;
    double defaultValue() const override;
    double value() const override;
    void setValue(double value) override;
    double stepSize() const override;
    std::uint32_t stepCount() const override;
    QString unit() const override;
public:
    QString valueToString(double value) const override;
    double stringToValue(const QString& string) const override;
private:
    AEffect* effect_;
    std::uint32_t index_;
    VestifalParameterProperties properties_;
};

class VestifalPluginParameter: public YADAW::Audio::Device::IAudioDeviceParameter
{
public:
    VestifalPluginParameter(AEffect* effect);
public:
    std::uint32_t parameterCount() const override;
    YADAW::Audio::Device::IParameter* parameter(std::uint32_t index) override;
    const YADAW::Audio::Device::IParameter* parameter(std::uint32_t index) const override;
public:

private:
    AEffect* effect_;
    std::vector<VestifalParameter> parameters_;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_VESTIFALPLUGINPARAMETER
