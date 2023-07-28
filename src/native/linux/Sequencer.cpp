#include "Sequencer.hpp"

namespace YADAW::Native
{
Sequencer::Sequencer()
{
    snd_seq_open(&seq_, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
    snd_seq_set_client_name(seq_, "YADAW application client");
}

Sequencer::~Sequencer()
{
    snd_seq_close(seq_);
}

Sequencer& Sequencer::instance()
{
    static Sequencer ret;
    return ret;
}

snd_seq_t* Sequencer::seq()
{
    return seq_;
}
}