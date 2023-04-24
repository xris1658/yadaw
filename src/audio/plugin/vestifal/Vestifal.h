#ifndef YADAW_SRC_AUDIO_PLUGIN_VESTIFAL_VESTIFAL
#define YADAW_SRC_AUDIO_PLUGIN_VESTIFAL_VESTIFAL

// Vestifal: FST and VeSTige combined
// FST: https://git.iem.at/zmoelnig/FST
// VeSTige: https://github.com/LMMS/lmms/blob/master/include/aeffectx.h

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

#if WIN32
#define VESTIFAL_CALL_CONV __cdecl
#else
#define VESTIFAL_CALL_CONV
#endif

#define VESTIFAL_ENTRY_NAME "VSTPluginMain"

inline int32_t CCONST(int32_t a, int32_t b, int32_t c, int32_t d)
{
    return (a << 24) + (b << 16) + (c << 8) + d;
}

// Audio master opcodes

enum AudioMasterOpcode
{
// Input: index, value; return 0
    audioMasterAutomate = 0,
// return 2400
    audioMasterVersion = 1,
// return shell plugin unique ID to create
    audioMasterGetCurrentUniqueId = 2,
    audioMasterIdle = 3,
// Input: index, value from bool (is output); return whether channel is valid from bool
    audioMasterPinConnected = 4,
// 5
// return 0
    audioMasterWantMIDI = 6,
// return TimeInfo*
    audioMasterGetTime = 7,
// In: ptr from Events*, return 0
    audioMasterProcessEvents = 8,
    audioMasterSetTime = 9,
// Input: value; return BPM * 10000
    audioMasterGetTempoAt = 10,
    audioMasterGetAutomatableParameterCount = 11,
    audioMasterGetParameterQuantization = 12,
    audioMasterIOChanged = 13,
    audioMasterNeedIdle = 14,
// Input: index (width), opt (height); return 1
    audioMasterSizeWindow = 15,
// return sample rate
    audioMasterGetSampleRate = 16,
// return block size
    audioMasterGetBlockSize = 17,
// `Steinberg::Vst::IAudioPresentationLatency`?
    audioMasterGetInputLatency = 18,
// `Steinberg::Vst::IAudioPresentationLatency`?
    audioMasterGetOutputLatency = 19,
    audioMasterGetPreviousPlug = 20,
    audioMasterGetNextPlug = 21,
    audioMasterWillReplaceOrAccumulate = 22,
    audioMasterGetCurrentProcessLevel = 23,
    audioMasterGetAutomationState = 24,
    audioMasterOfflineStart = 25,
    audioMasterOfflineRead = 26,
    audioMasterOfflineWrite = 27,
    audioMasterOfflineGetCurrentPass = 28,
    audioMasterOfflineGetCurrentMetaPass = 29,
    audioMasterSetOutputSampleRate = 30,
    audioMasterGetSpeakerArrangement = 31,
// Output: ptr as char*; return ptr
    audioMasterGetVendorName = 32,
// Output: ptr as char*; return ptr
    audioMasterGetProductName = 33,
    audioMasterGetVendorVersion = 34,
    audioMasterGetVendorSpecific = 35,
    audioMasterSetIcon = 36,
// Input: ptr as char*, return 0 or 1?
/* The following table shows strings used in `audioMasterCanDo`:
    | `ptr` as `const char*`                           | related `opcode` guessed from name |
    |:-------------------------------------------------|:-----------------------------------|
    | `"sendVstTimeInfo"`<br/>`"receiveVstTimeInfo"`   | `audioMasterGetTime`               |
    | `"sendVstMidiEvent"`<br/>`"receiveVstMidiEvent"` | `audioMasterProcessEvents`         |
    | `"sendVstEvents"`<br/>`"receiveVstEvents"`       | `audioMasterProcessEvents`         |
    | `"offline"`                                      | `audioMasterOffline*`              |
    | `"sizeWindow"`                                   | `audioMasterSizeWindow`            |
    | `"openFileSelector"`                             | `audioMasterOpenFileSelector`      |
    | `"closeFileSelector"`                            | `audioMasterCloseFileSelector`     |
    | `"supportShell"`                                 | `audioMasterGetCurrentUniqueId`    |
    | `"supplyIdle"`                                   | `audioMasterIdle`                  |
    | `"editFile"`                                     | `audioMasterEditFile`              |
    | `"acceptIOChanges"`                              | `audioMasterIOChanged`             |
    | `"asyncProcessing"`                              | **unknown**                        |
    | `"reportConnectionChanges"`                      | **unknown**                        |
 */
    audioMasterCanDo = 37,
    audioMasterGetLanguage = 38,
    audioMasterOpenWindow = 39,
    audioMasterCloseWindow = 40,
    audioMasterGetDirectory = 41,
    audioMasterUpdateDisplay = 42,
    audioMasterBeginEdit = 43,
    audioMasterEndEdit = 44,
    audioMasterOpenFileSelector = 45,
    audioMasterCloseFileSelector = 46,
    audioMasterEditFile = 47,
    audioMasterGetChunkFile = 48,
    audioMasterGetInputSpeakerArrangement = 49
};

enum EffectOpcode
{
    effectOpen = 0,
    effectClose = 1,
    effectSetProgram = 2,
    effectGetProgram = 3,
    effectSetProgramName = 4,
    effectGetProgramName = 5,
    effectGetParameterLabel = 6,
    effectGetParameterDisplay = 7,
    effectGetParameterName = 8,
// 9
    // Input: value
    effectSetSampleRate = 10,
    // Input: opt
    effectSetBlockSize = 11,
    // Input: input
    effectMainsChanges = 12,
    // Input: ptr from VestifalRectangle**
    effectGetRect = 13,
    // Input: ptr from window handle
    effectOpenEditor = 14,
    effectCloseEditor = 15,
// 16
// 17
// 18
    effectEditorIdle = 19,
    effectEditorTop = 20,
// 21
    effectGetUniqueId = 22,
    effectGetChunk = 23,
    effectSetChunk = 24,
    // Input: ptr from Events*
    effectProcessEvents = 25,
    // Input: index
    effectCanParameterBeAutomated = 26,
    // Input: index, ptr from char*
    effectStringToParameter = 27,
    effectGetProgramNameIndexed = 29,
// 30
// 31
// 32
    effectGetInputProperties = 33,
    effectGetOutputProperties = 34,
    // return PluginCategory
    effectGetPlugCategory = 35,
// 36
// 37
// 38
// 39
// 40
// 41
    effectSetSpeakerArrangement = 42,
// 43
// 44
    effectGetEffectName = 45,
// 46
    effectGetVendorName = 47,
    effectGetProductName = 48,
    effectGetVendorVersion = 49,
    effectGetVendorSpecific = 50,
    effectCanDo = 51,
// 52
// 53
// 54
// 55
// 56
// 57
    effectGetVersion = 58,
// 59
// 60
// 61
// 62
    effectGetCurrentMIDIProgram = 63,
// 64
// 65
    effectGetMIDINoteName = 66,
// 67
// 68
    effectGetSpeakerArrangement = 69,
    effectShellGetNextPlugin = 70,
    effectStartProcessing = 71,
    effectStopProcessing = 72,
    effectSetTotalSampleCountToProcess = 73,
// 74
// 75
// 76
    effectSetSamplePrecision = 77
};

enum EffectFlag
{
    effectHasEditor = 1 << 0,
    effectCanReplacing = 1 << 4,
    effectProgramChunks = 1 << 5,
    effectIsSynth = 1 << 8,
    effectNoSoundInStop = 1 << 9,
    effectCanProcessDoubleReplacing = 1 << 12
};

enum TimeInfoFlag
{
    transportChanged = 1 << 0,
    transportPlaying = 1 << 1,
    transportCycleActive = 1 << 2,
    transportRecording = 1 << 3,
    transportSystemTimeInNanosecondValid = 1 << 8,
    transportPPQPositionValid = 1 << 8,
    transportTempoValid = 1 << 9,
    transportBarsValid = 1 << 10,
    transportCyclePositionValid = 1 << 11,
    transportTimeSignatureValid = 1 << 12,
    transportSmpteValid = 1 << 13,
    transportClockValid = 1 << 14
};

struct TimeInfo
{
    double samplePosition;
    double sampleRate;
    double systemTimeInNanosecond;
    double ppqPosition;
    double tempo;
    double barStartPosition;
    double cycleStartPosition;
    double cycleEndPosition;
    int32_t timeSignatureNumerator;
    int32_t timeSignatureDenominator;
    int32_t smpteOffset;
    int32_t smpteFrameRate;
    int32_t samplesToNextClock;
    int32_t flags;
};

enum EventHeaderType
{
    EventHeaderMIDI = 1,
    EventHeaderSysEx = 6
};

enum Language
{
    LanguageEnglish = 1,
    LanguageGerman,
    LanguageFrench,
    LanguageItalian,
    LanguageSpanish,
    LanguageJapanese,
    LanguageKorean
};

enum VestifalProcessLevel
{
    ProcessLevelUnknown,
    ProcessLevel1,
    ProcessLevelRealtime,
    ProcessLevel3,
    ProcessLevelOffline
};

enum PluginCategory
{
    PluginUnknown = 0,
    PluginEffect = 1,
    PluginSynth = 2,
    PluginAnalyzer = 3,
    PluginMastering = 4,
    PluginSpatializer = 5,
    PluginRoomFX = 6,
    PluginSurroundFX = 7,
    PluginRestoration = 8,
    PluginOfflineProcess = 9,
    PluginShell = 10,
    PluginGenerator = 11
};

struct EventHeader
{
    int32_t type;
    int32_t byteSize;
    int32_t deltaFrames;
    int32_t flags;
};

struct MIDIEvent
{
    EventHeader header;
    int32_t noteLength;
    int32_t noteOffset;
    uint8_t data[4];
    int8_t detune;
    int8_t noteOffVelocity;
    int8_t reserved1;
    int8_t reserved2;
};

struct MIDISysExEvent
{
    EventHeader header;
    intptr_t size;
    intptr_t reserved1;
    uint8_t* data;
    intptr_t reserved2;
};

struct Events
{
    int32_t eventCount;
    intptr_t reserved;
    void* events[1];
};

struct VestifalRectangle
{
    int16_t top;
    int16_t left;
    int16_t bottom;
    int16_t right;
};

struct AEffect;

typedef intptr_t (VESTIFAL_CALL_CONV* AudioMasterDispatcherFunc)        (struct AEffect*, int32_t opcode, int32_t index, intptr_t opt, void* ptr, float value);
typedef intptr_t (VESTIFAL_CALL_CONV* AEffectDispatcherFunc)            (struct AEffect*, int32_t opcode, int32_t index, intptr_t opt, void* ptr, float value);
typedef void     (VESTIFAL_CALL_CONV* AEffectSetParameterFunc)          (struct AEffect*, int32_t index, float value);
typedef float    (VESTIFAL_CALL_CONV* AEffectGetParameterFunc)          (struct AEffect*, int32_t index);
typedef void     (VESTIFAL_CALL_CONV* AEffectProcessFunc)               (struct AEffect*, float** input, float** output, int32_t frameCount);
typedef void     (VESTIFAL_CALL_CONV* AEffectProcessReplacingFunc)      (struct AEffect*, float** input, float** output, int32_t frameCount);
typedef void     (VESTIFAL_CALL_CONV* AEffectProcessDoubleReplacingFunc)(struct AEffect*, double** input, double** output, int32_t frameCount);
typedef AEffect* (VESTIFAL_CALL_CONV* VestifalEntry)                    (AudioMasterDispatcherFunc);

struct AEffect
{
    int32_t magic; // CCONST('V', 's', 't', 'P')
    AEffectDispatcherFunc dispatcher;
    AEffectProcessFunc process;
    AEffectSetParameterFunc setParameter;
    AEffectGetParameterFunc getParameter;
    int32_t programCount;
    int32_t parameterCount;
    int32_t audioInputCount;
    int32_t audioOutputCount;
    int32_t flags;
    intptr_t reserved1;
    intptr_t reserved2;
    int32_t latencyInSamples;
    char padding[8];
    float unknownFloat;
    void* object;
    void* user;
    int32_t uniqueId;
    int32_t version;
    AEffectProcessReplacingFunc processReplacing;
    AEffectProcessDoubleReplacingFunc processDoubleReplacing;
};
#ifdef __cplusplus
}
#endif

#endif //YADAW_SRC_AUDIO_PLUGIN_VESTIFAL_VESTIFAL
