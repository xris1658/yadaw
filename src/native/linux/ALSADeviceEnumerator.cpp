#if(__linux__)

#include "ALSADeviceEnumerator.hpp"

#include "util/Base.hpp"

#include <alsa/asoundlib.h>
#include <alsa/control.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <cstring>

namespace YADAW::Native
{
using YADAW::Audio::Backend::ALSADeviceSelector;
std::vector<ALSADeviceSelector> audioInputDevices;
std::map<int, std::string> cardNames;
std::map<ALSADeviceSelector, std::string> audioDeviceNames;
std::vector<ALSADeviceSelector> audioOutputDevices;
std::vector<ALSADeviceSelector> midiDevices;
std::once_flag enumerateDeviceFlag;

void doEnumerateCardNames()
{
    int cardIndex = -1;
    while(snd_card_next(&cardIndex) == 0 && cardIndex != -1)
    {
        char* name = nullptr;
        if(snd_card_get_name(cardIndex, &name) == 0)
        {
            cardNames.emplace(cardIndex, std::string(name));
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
            ifs.getline(lineBuffer, YADAW::Util::stackArraySize(lineBuffer));
            ALSADeviceSelector selector(-1, -1);
            std::sscanf(lineBuffer, "%d-%d", &(selector.cIndex), &(selector.dIndex));
            if(selector.cIndex != -1 && selector.dIndex != -1)
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
}

void doEnumerateDevices()
{
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
                // pcmC0D0c or midiC0D0 (both shortest name)
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
                    if(std::strncmp(name, "midi", 4) == 0)
                    {
                        // midiC[0-9]+D[0-9]+
                        name += 5; // midiC
                        int cIndex = -1;
                        name += std::sscanf(name, "%d", &cIndex); // [0-9]+
                        ++name; // D
                        int dIndex = -1;
                        name += std::sscanf(name, "%d", &dIndex); // [0-9]+
                        if(cIndex != -1 && dIndex != -1)
                        {
                            midiDevices.emplace_back(cIndex, dIndex);
                        }
                    }
                }
            }
        }
    }
}

void ALSADeviceEnumerator::enumerateDevices()
{
    std::call_once(enumerateDeviceFlag, []()
    {
        doEnumerateDevices();
        std::sort(audioInputDevices.begin(), audioInputDevices.end());
        std::sort(audioOutputDevices.begin(), audioOutputDevices.end());
        std::sort(midiDevices.begin(), midiDevices.end());
        doEnumerateCardNames();
        doEnumerateDeviceNames();
    });
}

std::uint32_t ALSADeviceEnumerator::audioInputDeviceCount()
{
    enumerateDevices();
    return audioInputDevices.size();
}

std::uint32_t ALSADeviceEnumerator::audioOutputDeviceCount()
{
    enumerateDevices();
    return audioOutputDevices.size();
}

std::uint32_t ALSADeviceEnumerator::midiDeviceCount()
{
    enumerateDevices();
    return midiDevices.size();
}

std::optional<ALSADeviceSelector> ALSADeviceEnumerator::audioInputDeviceAt(std::uint32_t index)
{
    return index < audioInputDeviceCount()? std::optional(audioInputDevices[index]): std::nullopt;
}

std::optional<ALSADeviceSelector> ALSADeviceEnumerator::audioOutputDeviceAt(std::uint32_t index)
{
    return index < audioOutputDeviceCount()? std::optional(audioOutputDevices[index]): std::nullopt;
}

std::optional<ALSADeviceSelector> ALSADeviceEnumerator::midiDeviceAt(std::uint32_t index)
{
    return index < midiDeviceCount()? std::optional(midiDevices[index]): std::nullopt;
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
