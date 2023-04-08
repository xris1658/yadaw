#include "midi/MIDIInputDevice.hpp"

#include <QString>

#include <cstdio>
#include <cwchar>

void onReceiveMIDIMessage(const YADAW::MIDI::Message& message)
{
    auto data = message.data;
    if(YADAW::MIDI::isChannelVoiceMessageHeader(data[0]))
    {
        auto header = reinterpret_cast<const YADAW::MIDI::ChannelVoiceMessageHeader*>(data);
        switch(header->type)
        {
        case YADAW::MIDI::NoteOnMessage::TypeId:
        {
            auto* noteOn = reinterpret_cast<const YADAW::MIDI::NoteOnMessage*>(data);
            std::wprintf(L" Note on: %3hhu; velocity: %3hhu", noteOn->note, noteOn->velocity);
            if(noteOn->velocity == 0)
            {
                std::wprintf(L" (treat as note off)");
            }
            break;
        }
        case YADAW::MIDI::NoteOffMessage::TypeId:
        {
            auto* noteOff = reinterpret_cast<const YADAW::MIDI::NoteOffMessage*>(data);
            std::wprintf(L"Note off: %3hhu; velocity: %3hhu", noteOff->note, noteOff->velocity);
            break;
        }
        default:
            break;
        }
    }
    // std::wprintf(L"%d bytes: 0x", static_cast<int>(message.size));
    // for(int i = 0; i < message.size; ++i)
    // {
    //     std::wprintf(L"%.2hhx", message.data[i]);
    // }
    std::printf("\n");
}

int main()
{
    const auto& deviceInfoCollection = YADAW::MIDI::MIDIInputDevice::enumerateDevices();
    if(deviceInfoCollection.empty())
    {
        std::wprintf(L"No input devices available!");
        return 0;
    }
    for(const auto& deviceInfo: deviceInfoCollection)
    {
        std::wprintf(L"%d: %ls\n", static_cast<int>(&deviceInfo - deviceInfoCollection.data() + 1), reinterpret_cast<const wchar_t*>(deviceInfo.name.data()));
    }
    std::wprintf(L"Select input device: ");
    int index = -1;
    while(true)
    {
        std::wscanf(L"%d", &index);
        if(index > 0 && index <= deviceInfoCollection.size())
        {
            --index;
            break;
        }
        std::wprintf(L"Invalid index!\n");
    }
    YADAW::MIDI::MIDIInputDevice device(deviceInfoCollection[index].id);
    device.start(&onReceiveMIDIMessage);
    std::getchar();
    std::getchar();
    device.stop();
}