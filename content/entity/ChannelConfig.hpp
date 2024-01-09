#ifndef YADAW_CONTENT_ENTITY_CHANNELCONFIG
#define YADAW_CONTENT_ENTITY_CHANNELCONFIG

#include <QObject>

namespace YADAW::Entity
{
class ChannelConfig: public QObject
{
    Q_OBJECT
public:
    enum Config
    {
        Custom,
        Empty,
        Mono,
        Stereo,
        LRC,
        Quad,
        C50,
        C51,
        C61,
        C71
    };
    Q_ENUM(Config)
public:
    ChannelConfig(QObject* parent = nullptr);
    Q_INVOKABLE int channelCount(Config config);
};
}

#endif //YADAW_CONTENT_ENTITY_CHANNELCONFIG
