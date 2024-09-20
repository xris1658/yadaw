#ifndef YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER
#define YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER

#include "ui/WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QWindow>

namespace YADAW::UI
{
class NativePopupEventFilter: public QObject
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
    bool insert(QWindow& nativePopup, std::uint32_t index);
    void append(QWindow& nativePopup);
    bool remove(std::uint32_t index, std::uint32_t removeCount);
    void clear();
signals:
    void mousePressedOutside();
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    WindowAndId parentWindow_;
    std::vector<WindowAndId> nativePopups_;

};
}

#endif // YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER
