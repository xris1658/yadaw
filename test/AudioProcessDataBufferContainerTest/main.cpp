#include "audio/util/AudioBufferPool.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"

int main()
{
    constexpr auto singleBufferSize = 64;
    auto audioBufferPool = YADAW::Audio::Util::AudioBufferPool::createPool<float>(singleBufferSize);
    auto container = YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>();
    container.setSingleBufferSize(singleBufferSize);
    const auto& audioProcessData = container.audioProcessData();
    container.setInputGroupCount(2);
    container.setOutputGroupCount(1);
    for(int i = 0; i < audioProcessData.inputGroupCount; ++i)
    {
        container.setInputCount(i, 2);
        for(int j = 0; j < audioProcessData.inputCounts[i]; ++j)
        {
            container.setInputBuffer(i, j,
                std::make_shared<YADAW::Audio::Util::AudioBufferPool::Buffer>(
                    audioBufferPool->lend()
                )
            );
        }
    }
    for(int i = 0; i < audioProcessData.outputGroupCount; ++i)
    {
        container.setOutputCount(i, 1);
        for(int j = 0; j < audioProcessData.outputCounts[i]; ++j)
        {
            container.setOutputBuffer(i, j,
                container.inputBuffer(i, j)
            );
        }
    }
    for(int i = 0; i < audioProcessData.inputGroupCount; ++i)
    {
        for(int j = 0; j < audioProcessData.inputCounts[i]; ++j)
        {
            for(int k = 0; k < audioProcessData.singleBufferSize; ++k)
            {
                audioProcessData.inputs[i][j][k] = 1.0;
            }
        }
    }
}