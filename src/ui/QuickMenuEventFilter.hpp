#ifndef YADAW_SRC_UI_QUICKMENUEVENTFILTER
#define YADAW_SRC_UI_QUICKMENUEVENTFILTER

#include "ui/WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QQuickItem>

#include <set>
#include <vector>

namespace YADAW::UI
{
// I previously tried putting `Menu`s in `NativePopup`s. This class is used to
// handle events related to Qt Quick `MenuBar` correctly.
// Note that I used private implementations (see related sources in
// qtdeclarative/src/quicktemplates) instead of manually making one. It might
// not work on several versions of Qt.
class QuickMenuEventFilter: public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    QuickMenuEventFilter(QWindow& parentWindow);
    QuickMenuEventFilter(QWindow& parentWindow, QObject& menuBar);
    ~QuickMenuEventFilter();
public:
    const QWindow& parentWindow() const;
    QWindow& parentWindow();
    bool empty() const;
    std::uint32_t count() const;
    const QWindow& operator[](std::uint32_t index) const;
    QWindow& operator[](std::uint32_t index);
    const QWindow& at(std::uint32_t index) const;
    QWindow& at(std::uint32_t index);
public:
    bool insert(QWindow& nativePopup, std::uint32_t index, bool shouldReceiveKeyEvents);
    void append(QWindow& nativePopup, bool shouldReceiveKeyEvents);
    void popupShouldReceiveKeyEvents(std::uint32_t index, bool shouldReceiveKeyEvents);
    bool remove(std::uint32_t index, std::uint32_t removeCount);
    void clear();
private slots:
    void menuBarCountChanged();
private:
    void installMenuBarItemEventFilter();
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    WindowAndId parentWindow_;
    QObject* menuBar_;
    std::vector<WindowAndId> nativePopups_;
    std::vector<bool> shouldReceiveKeyEvents_;
    std::set<QQuickItem*> menuBarItems_;
    bool filteringMenuBarEvents_ = false;
};
}

#endif // YADAW_SRC_UI_QUICKMENUEVENTFILTER