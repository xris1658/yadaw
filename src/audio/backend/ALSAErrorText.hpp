#ifndef YADAW_SRC_AUDIO_BACKEND_ALSAERRORTEXT
#define YADAW_SRC_AUDIO_BACKEND_ALSAERRORTEXT

namespace YADAW::Audio::Backend
{
QString getIOString(bool isInput);

QString getOpenPCMErrorText();

QString getFillHardwareConfigSpaceText();

QString getSetSampleFormatText();

QString getSetSampleRateText();

QString getSetBufferSizeText();

QString getSetPeriodSizeText();

QString getGetChannelCountText();

QString getSetChannelCountText();

QString getSetSampleAccessText();

QString getDetermineHardwareConfigText();

QString getGetSotfwareConfigText();

QString getSetAvailMinText();

QString getSetStartThresholdText();

QString getDetermineSoftwareConfigText();

QString getAllocateBufferText();

QString getFinishText();

QString getALSAErrorText();

QString getALSADeviceNotFoundText();
}

#endif // YADAW_SRC_AUDIO_BACKEND_ALSAERRORTEXT
