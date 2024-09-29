#ifndef YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER
#define YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER

#include "ui/WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QWindow>

namespace YADAW::UI
{
class NativePopupEventFilter: public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    NativePopupEventFilter(QWindow& parentWindow);
    ~NativePopupEventFilter() override;
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
    std::vector<WindowAndId> nativePopups_;
    std::vector<bool> shouldReceiveKeyEvents_;
};
}

#endif // YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER
