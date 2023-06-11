#ifndef YADAW_CONTENT_ENTITY_AUDIOBACKENDSUPPORT
#define YADAW_CONTENT_ENTITY_AUDIOBACKENDSUPPORT

#include <QObject>

namespace YADAW::Entity
{
class AudioBackendSupport: public QObject
{
    Q_OBJECT
public:
    enum Backend
    {
        Off,
        Dummy,
        AudioGraph,
        ALSA,
        Last = ALSA
    };
    Q_ENUM(Backend)
public:
    AudioBackendSupport(QObject* parent = nullptr);
public:
    Q_INVOKABLE bool isBackendSupported(Backend backend) const;
};
}

#endif //YADAW_CONTENT_ENTITY_AUDIOBACKENDSUPPORT
