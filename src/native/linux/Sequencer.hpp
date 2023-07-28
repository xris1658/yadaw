#ifndef YADAW_SRC_NATIVE_LINUX_SEQUENCER
#define YADAW_SRC_NATIVE_LINUX_SEQUENCER

#include <alsa/asoundlib.h>

namespace YADAW::Native
{
class Sequencer
{
private:
    Sequencer();
public:
    ~Sequencer();
    static Sequencer& instance();
    snd_seq_t* seq();
private:
    snd_seq_t* seq_ = nullptr;
};
}

#endif //YADAW_SRC_NATIVE_LINUX_SEQUENCER
