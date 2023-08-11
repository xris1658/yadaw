#include "audio/engine/AudioProcessDataContainer.hpp"

int main()
{
    auto audioBufferPool = YADAW::Audio::Util::AudioBufferPool::createPool(64 * sizeof(float));
    auto processData = YADAW::Audio::Engine::AudioProcessDataContainer<float>(audioBufferPool);
    processData.setInputGroupCount(1);
    processData.setInputCount(0, 2);
    processData.setOutputGroupCount(1);
    processData.setOutputCount(0, 2);
    const auto& audioProcessData = processData.audioProcessData();
    for(int i = 0; i < audioProcessData.inputGroupCount; ++i)
    {
        for(int j = 0; j < audioProcessData.inputCounts[i]; ++j)
        {
            for(int k = 0; k < audioProcessData.singleBufferSize; ++k)
            {
                audioProcessData.inputs[i][j][k] = 0.0f;
            }
        }
    }
    for(int i = 0; i < audioProcessData.outputGroupCount; ++i)
    {
        for(int j = 0; j < audioProcessData.outputCounts[i]; ++j)
        {
            for(int k = 0; k < audioProcessData.singleBufferSize; ++k)
            {
                audioProcessData.outputs[i][j][k] = 0.0f;
            }
        }
    }
}