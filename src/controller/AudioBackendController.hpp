#ifndef YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER

namespace YADAW::Controller
{
enum AudioBackend
{
    Dummy,
    AudioGraph
};

void initializeAudioEngine(AudioBackend audioBackend);

}

#endif //YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
