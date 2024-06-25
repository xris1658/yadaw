#ifndef YADAW_CONTENT_ENTITY_IAUDIOIOPOSITION
#define YADAW_CONTENT_ENTITY_IAUDIOIOPOSITION

#include <QObject>

namespace YADAW::Entity
{
class IAudioIOPosition: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ getType)
    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    Q_PROPERTY(QString completeName READ getCompleteName NOTIFY completeNameChanged)
public:
    enum Type
    {
        Invalid,
        AudioHardwareIOChannel,
        BusAndFXChannel,
        PluginAuxIO
    };
    Q_ENUM(Type);
public:
    IAudioIOPosition(QObject* parent = nullptr): QObject(parent) {}
    ~IAudioIOPosition() override {}
public:
    virtual Type getType() const = 0;
    virtual QString getName() const = 0;
    virtual QString getCompleteName() const = 0;
signals:
    void nameChanged();
    void completeNameChanged();
};
}

#endif // YADAW_CONTENT_ENTITY_IAUDIOIOPOSITION
