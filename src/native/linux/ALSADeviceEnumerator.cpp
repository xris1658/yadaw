#include "ALSADeviceEnumerator.hpp"

#include <filesystem>
#include <mutex>

namespace YADAW::Native
{
std::vector<ALSADeviceEnumerator::DeviceSelector> audioInputDevices;
std::vector<ALSADeviceEnumerator::DeviceSelector> audioOutputDevices;
std::vector<ALSADeviceEnumerator::DeviceSelector> midiDevices;
std::once_flag enumerateDeviceFlag;

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
    std::call_once(enumerateDeviceFlag, &doEnumerateDevices);
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

std::optional<ALSADeviceEnumerator::DeviceSelector> ALSADeviceEnumerator::audioInputDeviceAt(std::uint32_t index)
{
    return index < audioInputDeviceCount()? std::optional(audioInputDevices[index]): std::nullopt;
}

std::optional<ALSADeviceEnumerator::DeviceSelector> ALSADeviceEnumerator::audioOutputDeviceAt(std::uint32_t index)
{
    return index < audioOutputDeviceCount()? std::optional(audioOutputDevices[index]): std::nullopt;
}

std::optional<ALSADeviceEnumerator::DeviceSelector> ALSADeviceEnumerator::midiDeviceAt(std::uint32_t index)
{
    return index < midiDeviceCount()? std::optional(midiDevices[index]): std::nullopt;
}
}