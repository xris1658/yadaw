#ifndef YADAW_CONTENT_CONTROL_METER
#define YADAW_CONTENT_CONTROL_METER

#include <QQuickItem>
#include <QSGRectangleNode>

#include <vector>

namespace YADAW::Control
{
class Meter: public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    Meter(int channelCount, QQuickWindow* window);
public:
    void onWindowBeforeRendering();
    void onWindowFrameSwapped();
private:
    std::vector<double> peakValues_;
    std::vector<double> meanValues_;
    std::vector<std::vector<QSGRectangleNode*>> nodes_;
};
}

#endif // YADAW_CONTENT_CONTROL_METER