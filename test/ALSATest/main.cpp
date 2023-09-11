#include "audio/backend/ALSABackend.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "midi/MIDIOutputDevice.hpp"
#include "native/linux/ALSADeviceEnumerator.hpp"
#include "util/IntegerRange.hpp"
#include "util/Endian.hpp"
#include "common/DisableStreamBuffer.hpp"

#include <cmath>
#include <cstdio>
#include <thread>

std::uint64_t totalFrameCount = 0;

using YADAW::Audio::Backend::ALSABackend;

void callback(const ALSABackend* backend,
    std::uint32_t inputCount, ALSABackend::AudioBuffer* inputBuffers,
    std::uint32_t outputCount, ALSABackend::AudioBuffer* outputBuffers);

int main(int argc, char** argv)
{
    disableStdOutBuffer();
    auto audioInputCount = YADAW::Audio::Backend::ALSABackend::audioInputDeviceCount();
    std::printf("Audio input device count: %u", audioInputCount);
    for(decltype(audioInputCount) i = 0; i < audioInputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioInputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(value.cIndex).value();
        std::printf("\n\t%u: %s (%s) (hw:%u,%u)", i + 1, name.c_str(), cardName.c_str(), value.cIndex, value.dIndex);
    }
    auto audioOutputCount = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceCount();
    std::printf("\n\nAudio output device count: %u", audioOutputCount);
    for(decltype(audioOutputCount) i = 0; i < audioOutputCount; ++i)
    {
        auto value = YADAW::Audio::Backend::ALSABackend::audioOutputDeviceAt(i).value();
        auto name = YADAW::Audio::Backend::ALSABackend::audioDeviceName(value).value();
        auto cardName = YADAW::Audio::Backend::ALSABackend::cardName(value.cIndex).value();
        std::printf("\n\t%u: %s (%s) (hw:%u,%u)", i + 1, name.c_str(), cardName.c_str(), value.cIndex, value.dIndex);
    }
    auto midiInputCount = YADAW::Native::ALSADeviceEnumerator::midiInputDevices().size();
    std::printf("\n\nMIDI input device count: %zu", midiInputCount);
    for(decltype(midiInputCount) i = 0; i < midiInputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiInputDevices()[i];
        std::printf("\n\t%zu: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    auto midiOutputCount = YADAW::Native::ALSADeviceEnumerator::midiOutputDevices().size();
    std::printf("\n\nMIDI output device count: %zu", midiOutputCount);
    for(decltype(midiOutputCount) i = 0; i < midiOutputCount; ++i)
    {
        auto value = YADAW::Native::ALSADeviceEnumerator::midiOutputDevices()[i];
        std::printf("\n\t%zu: %s (Client %u, Port %u)", i + 1, value.name.toStdString().c_str(), value.id.clientId, value.id.portId);
    }
    std::printf("\n\n");
    YADAW::Audio::Backend::ALSABackend backend;
    backend.initialize(44100, 512);
    YADAW::Audio::Backend::ALSADeviceSelector selector(0, 0);
    auto inputIndex = YADAW::Audio::Backend::findDeviceBySelector(backend, true, selector);
    auto outputIndex = findDeviceBySelector(backend, false, selector);
    if(inputIndex.has_value())
    {
        auto activateInputResult = backend.setAudioDeviceActivated(true, *inputIndex, true);
        if(activateInputResult == decltype(activateInputResult)::Failed)
        {
            std::printf("Activate default input failed!\n");
        }
    }
    if(outputIndex.has_value())
    {
        auto activateOutputResult = backend.setAudioDeviceActivated(false, *outputIndex, true);
        if(activateOutputResult == decltype(activateOutputResult)::Failed)
        {
            std::printf("Activate default output failed!\n");
        }
    }
    backend.start(callback);
    getchar();
    backend.stop();
}

auto freq = 440.0f;
auto ___2pi = 2.0f * std::acos(-1.0f);

void callback(const ALSABackend* backend,
    std::uint32_t inputCount, ALSABackend::AudioBuffer* inputBuffers,
    std::uint32_t outputCount, ALSABackend::AudioBuffer* outputBuffers)
{
    auto frameCount = backend->frameCount();
    auto cycle = backend->sampleRate() / freq;
    auto data = static_cast<float*>(alloca(sizeof(float) * frameCount));
    FOR_RANGE0(i, frameCount)
    {
        data[i] = std::sin(static_cast<float>(___2pi * (totalFrameCount + i) / cycle));
    }
    if(outputCount > 0)
    {
        auto& [format, interleaved, channelCount, buffer] = outputBuffers[0];
        if(interleaved)
        {
            auto interleavedBuffer = static_cast<std::byte*>(buffer.interleavedBuffer);
            FOR_RANGE0(i, frameCount)
            {
                switch(format)
                {
                case ALSABackend::SampleFormat::DoubleFloat:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<double*>(interleavedBuffer) + (i * channelCount + j)) = data[i];
                    }
                    break;
                }
                case ALSABackend::SampleFormat::DoubleFloatRE:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        auto ptr = reinterpret_cast<double*>(interleavedBuffer) + (i * channelCount + j);
                        *ptr = data[i];
                        YADAW::Util::reverseEndianness(ptr, sizeof(double));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Float:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<float*>(interleavedBuffer) + (i * channelCount + j)) = data[i];
                    }
                    break;
                }
                case ALSABackend::SampleFormat::FloatRE:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        auto ptr = reinterpret_cast<float*>(interleavedBuffer) + (i * channelCount + j);
                        *ptr = data[i];
                        YADAW::Util::reverseEndianness(ptr, sizeof(float));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int32:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<std::int32_t*>(interleavedBuffer) + (i * channelCount + j)) = (
                            static_cast<double>(data[i]) * static_cast<double>(0x80000000) - data[i]);
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int32RE:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        auto ptr = reinterpret_cast<std::int32_t*>(interleavedBuffer) + (i * channelCount + j);
                        *ptr = (static_cast<double>(data[i]) * static_cast<double>(0x80000000) - data[i]);
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int32_t));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int24:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<std::int32_t*>(interleavedBuffer) + (i * channelCount + j)) = (
                            data[i] * static_cast<float>(0x007fffff));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int24RE:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        auto ptr = reinterpret_cast<std::int32_t*>(interleavedBuffer) + (i * channelCount + j);
                        *ptr = (data[i] * static_cast<float>(0x007fffff));
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int32_t));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int20:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<std::int32_t*>(interleavedBuffer) + (i * channelCount + j)) = (
                            data[i] * static_cast<float>(0x0007ffff));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int20RE:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        auto ptr = reinterpret_cast<std::int32_t*>(interleavedBuffer) + (i * channelCount + j);
                        *ptr = (data[i] * static_cast<float>(0x0007ffff));
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int32_t));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int16:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<std::int16_t*>(interleavedBuffer) + (i * channelCount + j)) = (
                            data[i] * static_cast<float>(0x00007fff));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int16RE:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        auto ptr = reinterpret_cast<std::int16_t*>(interleavedBuffer) + (i * channelCount + j);
                        *ptr = (data[i] * static_cast<float>(0x00007fff));
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int16_t));
                    }
                    break;
                }
                case ALSABackend::SampleFormat::Int8:
                {
                    FOR_RANGE0(j, channelCount)
                    {
                        *(reinterpret_cast<std::int16_t*>(interleavedBuffer) + (i * channelCount + j)) = (
                            data[i] * static_cast<float>(0x0000007f));
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
        else
        {
            auto nonInterleavedArray = reinterpret_cast<std::byte**>(buffer.nonInterleavedBuffer);
            FOR_RANGE0(j, channelCount)
            {
                auto nonInterleavedBuffer = nonInterleavedArray[j];
                FOR_RANGE0(i, frameCount)
                {
                    switch(format)
                    {
                    case ALSABackend::SampleFormat::DoubleFloat:
                    {
                        *(reinterpret_cast<double*>(nonInterleavedBuffer) + i) = data[i];
                        break;
                    }
                    case ALSABackend::SampleFormat::DoubleFloatRE:
                    {
                        auto ptr = reinterpret_cast<double*>(nonInterleavedBuffer) + i;
                        *ptr = data[i];
                        YADAW::Util::reverseEndianness(ptr, sizeof(double));
                        break;
                    }
                    case ALSABackend::SampleFormat::Float:
                    {
                        *(reinterpret_cast<float*>(nonInterleavedBuffer) + i) = data[i];
                        break;
                    }
                    case ALSABackend::SampleFormat::FloatRE:
                    {
                        auto ptr = reinterpret_cast<float*>(nonInterleavedBuffer) + i;
                        *ptr = data[i];
                        YADAW::Util::reverseEndianness(ptr, sizeof(float));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int32:
                    {
                        *(reinterpret_cast<std::int32_t*>(nonInterleavedBuffer) + i) = (
                            static_cast<double>(data[i]) * static_cast<double>(0x80000000) - data[i]);
                        break;
                    }
                    case ALSABackend::SampleFormat::Int32RE:
                    {
                        auto ptr = reinterpret_cast<std::int32_t*>(nonInterleavedBuffer) + i;
                        *ptr = (static_cast<double>(data[i]) * static_cast<double>(0x80000000) - data[i]);
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int32_t));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int24:
                    {
                        *(reinterpret_cast<std::int32_t*>(nonInterleavedBuffer) + i) = (
                            data[i] * static_cast<float>(0x007fffff));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int24RE:
                    {
                        auto ptr = reinterpret_cast<std::int32_t*>(nonInterleavedBuffer) + i;
                        *ptr = (data[i] * static_cast<float>(0x007fffff));
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int32_t));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int20:
                    {
                        *(reinterpret_cast<std::int32_t*>(nonInterleavedBuffer) + i) = (
                            data[i] * static_cast<float>(0x0007ffff));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int20RE:
                    {
                        auto ptr = reinterpret_cast<std::int32_t*>(nonInterleavedBuffer) + i;
                        *ptr = (data[i] * static_cast<float>(0x0007ffff));
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int32_t));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int16:
                    {
                        *(reinterpret_cast<std::int16_t*>(nonInterleavedBuffer) + i) = (
                            data[i] * static_cast<float>(0x00007fff));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int16RE:
                    {
                        auto ptr = reinterpret_cast<std::int16_t*>(nonInterleavedBuffer) + i;
                        *ptr = (data[i] * static_cast<float>(0x00007fff));
                        YADAW::Util::reverseEndianness(ptr, sizeof(std::int16_t));
                        break;
                    }
                    case ALSABackend::SampleFormat::Int8:
                    {
                        *(reinterpret_cast<std::int16_t*>(nonInterleavedBuffer) + i) = (
                            data[i] * static_cast<float>(0x0000007f));
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }
    }
    totalFrameCount += frameCount;
}
