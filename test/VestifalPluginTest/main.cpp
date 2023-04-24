#include "audio/plugin/vestifal/Vestifal.h"
#include "native/Library.hpp"
#include "test/common/PluginWindowThread.hpp"

#include <QGuiApplication>

#include <cstdint>

std::int32_t uniqueId = 0;

std::thread::id audioThreadId;
std::thread::id mainThreadId;
std::thread::id idleThreadId;

std::intptr_t callback(AEffect* effect, std::int32_t opcode, std::int32_t input, std::intptr_t opt, void* ptr, float value)
{
    std::printf("%d\n", opcode);
    auto opcodeAsEnum = static_cast<AudioMasterOpcode>(opcode);
    switch(opcode)
    {
    case audioMasterAutomate:
        return 0;
    case audioMasterVersion:
        return 2400;
    case audioMasterGetCurrentUniqueId:
        return uniqueId;
    case audioMasterWantMIDI:
        return 1;
    case audioMasterGetSampleRate:
        return 48000;
    case audioMasterGetBlockSize:
        return 480;
    case audioMasterSizeWindow:
    {
        auto window = reinterpret_cast<QWindow*>(effect->user);
        window->setWidth(input);
        window->setHeight(opt);
        return 1;
    }
    case audioMasterGetCurrentProcessLevel:
    {
        if(auto threadId = std::this_thread::get_id(); threadId == audioThreadId)
        {
            return VestifalProcessLevel::ProcessLevelRealtime;
        }
        else
        {
            return VestifalProcessLevel::ProcessLevelUnknown;
        }
    }
    case audioMasterCanDo:
    {
        std::printf("%s\n", static_cast<const char*>(ptr));
        const auto* canDoString = static_cast<const char*>(ptr);
        if (std::strcmp(canDoString, "sizeWindow") == 0)
        {
            return 1;
        }
        return 0;
    }
    case audioMasterGetProductName:
        std::strcpy(reinterpret_cast<char*>(ptr), "TEST");
        return 0;
    case audioMasterGetVendorName:
        std::strcpy(reinterpret_cast<char*>(ptr), "xris1658");
        return 0;
    case audioMasterUpdateDisplay:
        effect->dispatcher(effect, EffectOpcode::effectEditorIdle, 0, 0, nullptr, 0.0f);
        return 0;
    }
    return 0;
}

int main(int argc, char** argv)
{
    QGuiApplication application(argc, argv);
    mainThreadId = std::this_thread::get_id();
    YADAW::Native::Library library("C:\\VST64\\VST64\\Grand Piano XXL VST\\Grand Piano XXL.dll");
    auto entry = library.getExport<VestifalEntry>(VESTIFAL_ENTRY_NAME);
    if(!entry)
    {
        entry = library.getExport<VestifalEntry>("main");
    }
    if(entry)
    {
        auto effect = entry(&callback);
        effect->dispatcher(effect, EffectOpcode::effectOpen, 0, 0, nullptr, 0.0f);
        effect->dispatcher(effect, EffectOpcode::effectSetSampleRate, 0, 0, nullptr, 48000.0f);
        effect->dispatcher(effect, EffectOpcode::effectSetBlockSize, 0, 480, nullptr, 0.0f);
        VestifalSpeakerArrangement* inputSpeakerArrangements;
        VestifalSpeakerArrangement* outputSpeakerArrangements;
        effect->dispatcher(effect, EffectOpcode::effectGetSpeakerArrangement, 0, reinterpret_cast<intptr_t>(&inputSpeakerArrangements), &outputSpeakerArrangements, 0.0f);
        PluginWindowThread pluginWindowThread;
        pluginWindowThread.start();
        auto* window = pluginWindowThread.window();
        effect->user = window;
        window->show();
        effect->dispatcher(effect, EffectOpcode::effectOpenEditor, 0, 0, reinterpret_cast<void*>(window->winId()), 0.0f);
        effect->dispatcher(effect, EffectOpcode::effectMainsChanges, 0, 1, nullptr, 0.0f);
        effect->dispatcher(effect, EffectOpcode::effectStartProcessing, 0, 0, nullptr, 0.0f);
        VestifalRectangle* rectangle = nullptr;
        effect->dispatcher(effect, EffectOpcode::effectGetRect, 0, 0, &rectangle, 0.0f);
        window->setWidth(rectangle->right - rectangle->left);
        window->setHeight(rectangle->bottom - rectangle->top);
        std::vector<float*> pInput(effect->audioInputCount, nullptr);
        std::vector<std::vector<float>> inputs(effect->audioInputCount, std::vector<float>(480, 0.0));
        for (int i = 0; i < pInput.size(); ++i)
        {
            pInput[i] = inputs[i].data();
            for (int j = 0; j < 480; ++j)
            {
                inputs[i][j] = 0.0625 * std::sin(j * 2 * std::acos(-1) / 32.0);
            }
        }
        std::vector<float*> pOutput(effect->audioOutputCount, nullptr);
        std::vector<std::vector<float>> outputs(effect->audioOutputCount, std::vector<float>(480, 0.0));
        for (int i = 0; i < pOutput.size(); ++i)
        {
            pOutput[i] = outputs[i].data();
        }
        std::atomic_bool stop{ false };
        std::thread idleThread([effect, &stop]()
        {
            idleThreadId = std::this_thread::get_id();
            while(!stop.load(std::memory_order::memory_order_acquire))
            {
                effect->dispatcher(effect, EffectOpcode::effectEditorIdle, 0, 0, nullptr, 0.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        });
        std::thread audioThread([effect, &pInput, &pOutput, &stop]()
        {
            audioThreadId = std::this_thread::get_id();
            while (!stop.load(std::memory_order::memory_order_acquire))
            {
                auto sleepUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(10);
                effect->processReplacing(effect, pInput.data(), pOutput.data(), 480);
                while (std::chrono::steady_clock::now() < sleepUntil)
                {
                    std::this_thread::yield();
                }
            }
        });
        QGuiApplication::exec();
        stop.store(true, std::memory_order::memory_order_release);
        audioThread.join();
        idleThread.join();
        effect->dispatcher(effect, EffectOpcode::effectCloseEditor, 0, 0, nullptr, 0.0f);
        effect->dispatcher(effect, EffectOpcode::effectStopProcessing, 0, 0, nullptr, 0.0f);
        effect->dispatcher(effect, EffectOpcode::effectMainsChanges, 0, 0, nullptr, 0.0f);
        effect->dispatcher(effect, EffectOpcode::effectClose, 0, 0, nullptr, 0.0f);
    }
}