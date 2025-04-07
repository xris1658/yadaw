#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFString.h>
#include <../../System/Library/Frameworks/AudioToolbox.framework/Headers/AudioQueue.h>

#include <array>
#include <atomic>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <thread>

std::uint32_t channelCount = 2;

std::uint32_t sampleByteSize = sizeof(float);

std::uint32_t frameCount = 512;

struct AudioQueueContext
{
    AudioStreamBasicDescription        desc;
    std::array<AudioQueueBufferRef, 2> buffers;
    AudioQueueRef                      queue;
    std::atomic_flag                   running;
};

std::uint64_t sampleOffset = 0;

double freq = 440.0;

void callback(void* inUserData, AudioQueueBufferRef inBuffer)
{
    const auto& context = *static_cast<const AudioQueueContext*>(inUserData);
    auto cycle = context.desc.mSampleRate / freq;
    const auto _2pi = 2.0f * std::acos(-1.0f);
    inBuffer->mAudioDataByteSize = frameCount * context.desc.mBytesPerFrame;
    auto data = static_cast<float*>(inBuffer->mAudioData);
    for(std::uint32_t i = 0; i < frameCount; ++i)
    {
        auto value = std::sin(_2pi * (sampleOffset + i) / cycle);
        for(std::uint32_t j = 0; j < channelCount; ++j)
        {
            data[i * channelCount + j] = value;
        }
    }
    sampleOffset += frameCount;
}

void audioQueueOutputCallback(
    void* inUserData,
    AudioQueueRef inAQ,
    AudioQueueBufferRef inBuffer)
{
    const auto& context = *static_cast<const AudioQueueContext*>(inUserData);
    callback(inUserData, inBuffer);
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nullptr);
}

int main()
{
    std::atomic<CFRunLoopRef> audioThreadRunLoop = nullptr;
    AudioQueueContext context {
        .desc = AudioStreamBasicDescription {
            .mSampleRate       = 44100.0,
            .mFormatID         = kAudioFormatLinearPCM,
            .mFormatFlags      = kLinearPCMFormatFlagIsFloat | kLinearPCMFormatFlagIsPacked,
            .mBytesPerPacket   = channelCount * sampleByteSize,
            .mFramesPerPacket  = 1,
            .mBytesPerFrame    = channelCount * sampleByteSize,
            .mChannelsPerFrame = channelCount,
            .mBitsPerChannel   = sizeof(float) * CHAR_BIT
        }
    };
    std::thread audioThread(
        [&]()
        {
            audioThreadRunLoop.store(CFRunLoopGetCurrent(), std::memory_order_release);
            while(!context.running.test(std::memory_order_acquire)) {}
            CFRunLoopRun();
        }
    );
    while(!audioThreadRunLoop.load(std::memory_order_acquire)) {}
    AudioQueueNewOutput(
        &context.desc,
        &audioQueueOutputCallback,
        &context,
        audioThreadRunLoop.load(std::memory_order_acquire),
        kCFRunLoopCommonModes,
        0,
        &context.queue
    );
    for(auto& buffer: context.buffers)
    {
        AudioQueueAllocateBuffer(context.queue, channelCount * sampleByteSize * frameCount, &buffer);
        callback(&context, buffer);
        AudioQueueEnqueueBuffer(context.queue, buffer, 0, nullptr);
    }
    AudioQueueStart(context.queue, nullptr);
    context.running.test_and_set(std::memory_order_release);
    std::getchar();
    context.running.clear(std::memory_order_release);
    AudioQueueFlush(context.queue);
    AudioQueueStop(context.queue, true);
    audioThread.join();
    auto beginsToStop = std::chrono::steady_clock::now();
    CFRunLoopStop(audioThreadRunLoop);
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - beginsToStop;
    std::printf("`CFRunLoopStop` spent %lf seconds\n", duration.count());
    for(auto& buffer: context.buffers)
    {
        AudioQueueFreeBuffer(context.queue, buffer);
    }
    AudioQueueDispose(context.queue, true);
}