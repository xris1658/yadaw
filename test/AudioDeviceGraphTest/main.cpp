#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/util/SampleDelay.hpp"
#include "audio/util/Summing.hpp"

#include <execution>
#include <random>
#include <vector>

int main()
{
    std::random_device randomDevice;
    auto bufferSize = 64;
    std::vector<float> source(bufferSize, 0.0f);
    for(auto& data: source)
    {
        data = randomDevice() / static_cast<float>(std::random_device::max() - std::random_device::min());
    }
    YADAW::Audio::Engine::AudioDeviceGraph graph;
    YADAW::Audio::Util::AudioChannelGroup audioChannelGroup;
    audioChannelGroup.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::Stereo);
    std::vector<YADAW::Audio::Util::SampleDelay> sd;
    auto deviceCount = 8;
    sd.reserve(deviceCount);
    std::vector<ade::NodeHandle> nodes;
    auto channelCount = audioChannelGroup.channelCount();
    std::vector<std::vector<std::vector<float>>> d3(
        deviceCount,
        std::vector<std::vector<float>>(
            channelCount,
            std::vector<float>(
                bufferSize, 0.0f
            )
        )
    );
    std::vector<std::vector<float*>> d2(
        deviceCount,
        std::vector<float*>(
            channelCount,
            nullptr
        )
        );
    std::vector<std::uint32_t> c1(
        deviceCount,
        channelCount
    );
    std::vector<float**> d1(
        deviceCount, nullptr
    );
    std::vector<YADAW::Audio::Device::AudioProcessData<float>> a1(deviceCount);
    for(int i = 0; i < deviceCount; ++i)
    {
        d2.emplace_back();
        for(int j = 0; j < channelCount; ++j)
        {
            d2[i][j] = d3[i][j].data();
        }
        d1[i] = d2[i].data();
        sd.emplace_back(i * 5 % deviceCount, audioChannelGroup);
        sd.back().startProcessing();
        a1[i].singleBufferSize = bufferSize;
        a1[i].inputGroupCount = 1;
        a1[i].inputCounts = c1.data() + i;
        a1[i].inputs = d1.data() + i;
        a1[i].outputGroupCount = 1;
        a1[i].outputCounts = c1.data() + i;
        a1[i].outputs = d1.data() + i;
    }
    for(int i = 0; i < deviceCount; ++i)
    {
        nodes.emplace_back(graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(sd[i]), std::move(a1[i])));
    }
    YADAW::Audio::Util::Summing summing(deviceCount, YADAW::Audio::Base::ChannelGroupType::Stereo);
    std::vector<std::vector<float>> sd3(
        channelCount, std::vector<float>(bufferSize, 0.0f)
    );
    std::vector<float*> sd2(channelCount, nullptr);
    for(int i = 0; i < channelCount; ++i)
    {
        sd2[i] = sd3[i].data();
    }
    std::uint32_t sc1 = 2;
    float** sd1 = sd2.data();
    YADAW::Audio::Device::AudioProcessData<float> spd;
    spd.singleBufferSize = bufferSize;
    spd.inputGroupCount = d1.size();
    spd.inputCounts = c1.data();
    spd.inputs = d1.data();
    spd.outputGroupCount = 1;
    spd.outputCounts = &sc1;
    spd.outputs = &sd1;
    auto summingNode = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(summing), std::move(spd));
    for(int i = 0; i < deviceCount; ++i)
    {
        graph.connect(nodes[i], summingNode, 0, i);
    }
    auto optionalTopo = graph.topologicalOrder();
    assert(optionalTopo.has_value());
    auto& topo = optionalTopo.value();
    for(int i = 0; i < 10; ++i)
    {
        for(int j = 0; j < deviceCount; j += 2)
        {
            for(int k = 0; k < channelCount; ++k)
            {
                std::copy(source.begin(), source.end(), d3[j][k].begin());
                std::copy(source.begin(), source.end(), d3[j + 1][k].begin());
                std::for_each(d3[j + 1][k].begin(), d3[j + 1][k].end(),
                    [](float& value) { value *= -1; });
            }
        }
        for(auto& node: topo)
        {
            std::for_each(std::execution::par_unseq, node.begin(), node.end(),
                [](YADAW::Audio::Engine::AudioDeviceGraph::AudioDeviceProcessNode& n)
                {
                    n.doProcess();
                }
            );
        }
        assert(
            std::all_of(
                sd3.begin(), sd3.end(),
                [](const std::vector<float>& vec)
                {
                    return std::all_of(
                        vec.begin(), vec.end(),
                        [](float value)
                        {
                            return value == 0.0f;
                        }
                    );
                }
            )
        );
    }
}