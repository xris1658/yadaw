#include "entity/AudioBackendSupport.hpp"

#include "audio/backend/AudioBackendSupport.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
AudioBackendSupport::AudioBackendSupport(QObject* parent): QObject(parent)
{
    YADAW::Util::setCppOwnership(*this);
}

Q_INVOKABLE bool AudioBackendSupport::isBackendSupported(YADAW::Entity::AudioBackendSupport::Backend backend) const
{
    switch(backend)
    {
    case Backend::Off:
        return YADAW::Audio::Backend::isBackendSupported<YADAW::Audio::Backend::AudioBackend::Off>;
    case Backend::AudioGraph:
        return YADAW::Audio::Backend::isBackendSupported<YADAW::Audio::Backend::AudioBackend::AudioGraph>;
    case Backend::ALSA:
        return YADAW::Audio::Backend::isBackendSupported<YADAW::Audio::Backend::AudioBackend::ALSA>;
    case Backend::WASAPIExclusive:
        return YADAW::Audio::Backend::isBackendSupported<YADAW::Audio::Backend::AudioBackend::WASAPIExclusive>;
    }
    return false;
}
}
