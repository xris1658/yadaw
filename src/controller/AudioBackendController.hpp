#ifndef YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
enum AudioBackend
{
    Dummy,
    AudioGraph,
    ALSA,
    Invalid
};

class AudioBackendController
{
public:
    AudioBackend audioBackend() const;
public:
    bool importConfiguration(const YAML::Node& node);
    YAML::Node exportConfiguration() const;
private:
    AudioBackend audioBackend_ = AudioBackend::Dummy;
};
}

#endif //YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
