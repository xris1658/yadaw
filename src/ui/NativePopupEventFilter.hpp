#ifndef YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER
#define YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER

#include <QObject>
#include <QAbstractNativeEventFilter>

namespace YADAW::UI
{
class NativePopupEventFilter: public QObject, public QAbstractNativeEventFilter
{
public:
    NativePopupEventFilter(QWindow& parentWindow);
    ~NativePopupEventFilter() override;
public:
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
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    QWindow* parentWindow_;
    std::vector<QWindow*> nativePopups_;
};
}

#endif // YADAW_SRC_UI_NATIVEPOPUPEVENTFILTER
