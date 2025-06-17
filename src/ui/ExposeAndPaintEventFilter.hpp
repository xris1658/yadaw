#ifndef YADAW_SRC_UI_EXPOSEANDPAINTEVENTFILTER
#define YADAW_SRC_UI_EXPOSEANDPAINTEVENTFILTER

#include <QObject>
#include <QRegion>

class QWindow;

namespace YADAW::UI
{
// https://lists.qt-project.org/pipermail/development/2015-January/019730.html
class ExposeAndPaintEventFilter: public QObject
{
    Q_OBJECT
private:
    ExposeAndPaintEventFilter(QWindow& window);
public:
    static ExposeAndPaintEventFilter& createIfNeeded(QWindow& window);
    ~ExposeAndPaintEventFilter();
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    QWindow* window_;
    QRegion prevRegion_;
};
}

#endif // YADAW_SRC_UI_EXPOSEANDPAINTEVENTFILTER