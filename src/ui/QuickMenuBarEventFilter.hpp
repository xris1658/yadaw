#ifndef YADAW_SRC_UI_QUICKMENUBAREVENTFILTER
#define YADAW_SRC_UI_QUICKMENUBAREVENTFILTER

#include "ui/WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QWindow>

#include <vector>

namespace YADAW::UI
{
class QuickMenuBarEventFilter: public QObject, public QAbstractNativeEventFilter
{
public:
    QuickMenuBarEventFilter(QWindow& parentWindow, QObject& menuBar);
    ~QuickMenuBarEventFilter();
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
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    WindowAndId parentWindow_;
    QObject* menuBar_;
    std::vector<WindowAndId> nativePopups_;
    std::vector<bool> shouldReceiveKeyEvents_;
};
}

#endif // YADAW_SRC_UI_QUICKMENUBAREVENTFILTER