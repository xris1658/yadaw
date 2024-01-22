#ifndef YADAW_CONTENT_ENTITY_CHANNELINFO
#define YADAW_CONTENT_ENTITY_CHANNELINFO

#include <QColor>
#include <QObject>
#include <QString>

namespace YADAW::Entity
{
class ChannelInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER name_ READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QColor color MEMBER color_ READ color WRITE setColor NOTIFY colorChanged)
public:
    ChannelInfo(QObject* parent = nullptr);
    ChannelInfo(const ChannelInfo& rhs);
    ~ChannelInfo();
public:
    const QString& name() const;
    QColor color() const;
    void setName(const QString& name);
    void setColor(QColor color);
signals:
    void nameChanged();
    void colorChanged();
private:
    QString name_;
    QColor color_;
};
}

#endif //YADAW_CONTENT_ENTITY_CHANNELINFO
