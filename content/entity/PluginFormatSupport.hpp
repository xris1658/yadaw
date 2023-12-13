#ifndef YADAW_CONTENT_ENTITY_PLUGINFORMATSUPPORT
#define YADAW_CONTENT_ENTITY_PLUGINFORMATSUPPORT

#include <QObject>

namespace YADAW::Entity
{
class PluginFormatSupport: public QObject
{
    Q_OBJECT
public:
    enum PluginFormat
    {
        VST3,
        CLAP,
        Vestifal
    };
    Q_ENUM(PluginFormat)
public:
    PluginFormatSupport(QObject* parent = nullptr);
public:
    Q_INVOKABLE bool isPluginFormatSupported(PluginFormat pluginFormat) const;
};
}

#endif //YADAW_CONTENT_ENTITY_PLUGINFORMATSUPPORT
