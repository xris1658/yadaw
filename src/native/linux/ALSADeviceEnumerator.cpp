#if __linux__

#include "ALSADeviceEnumerator.hpp"

#include "native/linux/Sequencer.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"

#include <alsa/asoundlib.h>
#include <alsa/control.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <cstring>

using YADAW::Audio::Backend::ALSADeviceSelector;
std::vector<ALSADeviceSelector> audioInputDevices;
std::map<int, std::string> cardNames;
std::map<ALSADeviceSelector, std::string> audioDeviceNames;
std::vector<ALSADeviceSelector> audioOutputDevices;
std::vector<YADAW::MIDI::DeviceInfo> midiInputDevices;
std::vector<YADAW::MIDI::DeviceInfo> midiOutputDevices;
std::once_flag enumerateDeviceFlag;

namespace YADAW::Native
{
void doEnumerateCardNames()
{
    int cardIndex = -1;
    while(snd_card_next(&cardIndex) == 0 && cardIndex != -1)
    {
        char* name = nullptr;
        if(snd_card_get_name(cardIndex, &name) == 0)
        {
            cardNames.emplace(cardIndex, std::string(name));
            free(name);
        }
    }
}

void doEnumerateDeviceNames()
{
    // Read device names without doing `snd_pcm_open` that potentially fails
    std::ifstream ifs("/proc/asound/pcm", std::ios::in);
    if(!(ifs.fail()))
    {
        char lineBuffer[128];
        while(true)
        {
            ifs.getline(lineBuffer, std::size(lineBuffer));
            ALSADeviceSelector selector(-1, -1);
            std::sscanf(lineBuffer, "%d-%d", &(selector.cIndex), &(selector.dIndex));
            if(selector.cIndex != UINT32_MAX && selector.dIndex != UINT32_MAX)
            {
                auto name = std::strchr(lineBuffer + 7, ':') + YADAW::Util::stringLength(": ");
                auto nameEnd = std::strchr(name + 2, ':');
                audioDeviceNames.emplace(selector, std::string(name, nameEnd - name - 1));
            }
            if(ifs.rdstate() & std::ios::eofbit)
            {
                break;
            }
        }
    }
    ifs.close();
    if(!midiInputDevices.empty())
    {
        auto seq = Sequencer::instance().seq();
        for(auto& midiDevice: midiInputDevices)
        {
            snd_seq_client_info_t* clientInfo = nullptr;
            snd_seq_client_info_alloca(&clientInfo);
            snd_seq_port_info_t* portInfo = nullptr;
            snd_seq_port_info_alloca(&portInfo);
            snd_seq_get_any_client_info(seq, midiDevice.id.clientId, clientInfo);
            snd_seq_get_any_port_info(seq, midiDevice.id.clientId, midiDevice.id.portId, portInfo);
            midiDevice.name = QString("%1 (%2)").arg(
                QString::fromLocal8Bit(snd_seq_port_info_get_name(portInfo)),
                QString::fromLocal8Bit(snd_seq_client_info_get_name(clientInfo))
            );
        }
        for(auto& midiDevice: midiOutputDevices)
        {
            snd_seq_client_info_t* clientInfo = nullptr;
            snd_seq_client_info_alloca(&clientInfo);
            snd_seq_port_info_t* portInfo = nullptr;
            snd_seq_port_info_alloca(&portInfo);
            snd_seq_get_any_client_info(seq, midiDevice.id.clientId, clientInfo);
            snd_seq_get_any_port_info(seq, midiDevice.id.clientId, midiDevice.id.portId, portInfo);
            midiDevice.name = QString("%1 (%2)").arg(
                QString::fromLocal8Bit(snd_seq_port_info_get_name(portInfo)),
                QString::fromLocal8Bit(snd_seq_client_info_get_name(clientInfo))
            );
        }
    }
}

void doEnumerateDevices()
{
    if(!std::filesystem::exists("/proc/asound"))
    {
        return;
    }
    const char* path = "/dev/snd";
    std::filesystem::directory_entry directoryEntry(path);
    if(directoryEntry.exists())
    {
        std::filesystem::directory_iterator it(path);
        for(const auto& i: it)
        {
            if(i.exists() && !i.is_directory())
            {
                auto name = i.path().filename().c_str();
                // pcmC0D0c
                if(std::strlen(name) >= 8)
                {
                    if(std::strncmp(name, "pcm", 3) == 0)
                    {
                        // pcmC[0-9]+D[0-9]+[cp]
                        name += 4; // pcmC
                        int cIndex = -1;
                        name += std::sscanf(name, "%d", &cIndex); // [0-9]+
                        ++name; // D
                        int dIndex = -1;
                        name += std::sscanf(name, "%d", &dIndex); // [0-9]+
                        if(cIndex != -1 && dIndex != -1)
                        {
                            if(*name == 'c')
                            {
                                audioInputDevices.emplace_back(cIndex, dIndex);
                            }
                            else if(*name == 'p')
                            {
                                audioOutputDevices.emplace_back(cIndex, dIndex);
                            }
                        }
                    }
                }
            }
        }
    }
    std::vector<std::pair<int, int>> clients;
    snd_seq_client_info_t* clientInfo = nullptr;
    snd_seq_client_info_alloca(&clientInfo);
    snd_seq_port_info_t* portInfo = nullptr;
    snd_seq_port_info_alloca(&portInfo);
    snd_seq_client_info_set_client(clientInfo, -1);
    auto seq = Sequencer::instance().seq();
    auto seqClientId = snd_seq_client_id(seq);
    while(snd_seq_query_next_client(seq, clientInfo) >= 0)
    {
        auto clientId = snd_seq_client_info_get_client(clientInfo);
        if(clientId != seqClientId)
        {
            auto portCount = snd_seq_client_info_get_num_ports(clientInfo);
            snd_seq_port_info_set_client(portInfo, clientId);
            snd_seq_port_info_set_port(portInfo, -1);
            FOR_RANGE0(i, portCount)
            {
                snd_seq_query_next_port(seq, portInfo);
                auto portId = snd_seq_port_info_get_port(portInfo);
                auto capability = snd_seq_port_info_get_capability(portInfo);
                if(capability & (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
                {
                    midiInputDevices.emplace_back(
                        YADAW::MIDI::DeviceInfo{
                            {
                                static_cast<std::uint32_t>(clientId),
                                static_cast<std::uint32_t>(portId)
                            },
                            QString()
                        }
                    );
                }
                if(capability & (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE))
                {
                    midiOutputDevices.emplace_back(
                        YADAW::MIDI::DeviceInfo{
                            {
                                static_cast<std::uint32_t>(clientId),
                                static_cast<std::uint32_t>(portId)
                            },
                            QString()
                        }
                    );
                }
            }
        }
    }
}

bool compareMIDIDeviceInfo(const YADAW::MIDI::DeviceInfo& lhs, const YADAW::MIDI::DeviceInfo& rhs)
{
    return lhs.id < rhs.id;
}

void ALSADeviceEnumerator::enumerateDevices()
{
    std::call_once(enumerateDeviceFlag, []()
    {
        doEnumerateDevices();
        std::sort(::audioInputDevices.begin(), ::audioInputDevices.end());
        std::sort(::audioOutputDevices.begin(), ::audioOutputDevices.end());
        std::sort(::midiInputDevices.begin(), ::midiInputDevices.end(),
            &compareMIDIDeviceInfo
        );
        std::sort(::midiOutputDevices.begin(), ::midiOutputDevices.end(),
            &compareMIDIDeviceInfo
        );
        doEnumerateCardNames();
        doEnumerateDeviceNames();
    });
}

const std::vector<ALSADeviceSelector>& ALSADeviceEnumerator::audioInputDevices()
{
    enumerateDevices();
    return ::audioInputDevices;
}

const std::vector<ALSADeviceSelector>& ALSADeviceEnumerator::audioOutputDevices()
{
    enumerateDevices();
    return ::audioOutputDevices;
}

const std::vector<YADAW::MIDI::DeviceInfo> ALSADeviceEnumerator::midiInputDevices()
{
    enumerateDevices();
    return ::midiInputDevices;
}

const std::vector<YADAW::MIDI::DeviceInfo> ALSADeviceEnumerator::midiOutputDevices()
{
    enumerateDevices();
    return ::midiOutputDevices;
}

std::optional<std::string> ALSADeviceEnumerator::audioDeviceName(ALSADeviceSelector selector)
{
    enumerateDevices();
    if(auto it = audioDeviceNames.find(selector); it != audioDeviceNames.end())
    {
        return {it->second};
    }
    return std::nullopt;
}

std::optional<std::string> ALSADeviceEnumerator::cardName(int cardIndex)
{
    enumerateDevices();
    if(auto it = cardNames.find(cardIndex); it != cardNames.end())
    {
        return {it->second};
    }
    return std::nullopt;
}
}

#endif
