#include "NativePopupEventFilter.hpp"

#include <QCoreApplication>
#include <QKeyEvent>

namespace YADAW::UI
{
void showWindowWithoutActivating(QWindow& window)
{
#if _WIN32
    auto hwnd = reinterpret_cast<HWND>(window.winId());
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
#endif
}

NativePopupEventFilter::NativePopupEventFilter(QWindow& parentWindow):
    QObject(nullptr),
    QAbstractNativeEventFilter(),
    parentWindow_(&parentWindow)
{
    parentWindow.installEventFilter(this);
    QCoreApplication::instance()->installNativeEventFilter(this);
}

NativePopupEventFilter::~NativePopupEventFilter()
{
    QCoreApplication::instance()->installNativeEventFilter(this);
    parentWindow_->installEventFilter(this);
}

bool NativePopupEventFilter::empty() const
{
    return nativePopups_.empty();
}

std::uint32_t NativePopupEventFilter::count() const
{
    return nativePopups_.size();
}

const QWindow& NativePopupEventFilter::operator[](std::uint32_t index) const
{
    return *nativePopups_[index];
}

QWindow& NativePopupEventFilter::operator[](std::uint32_t index)
{
    return *nativePopups_[index];
}

const QWindow& NativePopupEventFilter::at(std::uint32_t index) const
{
    return *nativePopups_.at(index);
}

QWindow& NativePopupEventFilter::at(std::uint32_t index)
{
    return *nativePopups_.at(index);
}

bool NativePopupEventFilter::insert(QWindow& nativePopup, std::uint32_t index)
{
    if(index < count())
    {
        nativePopups_.insert(nativePopups_.begin() + index, &nativePopup);
        return true;
    }
    return false;
}

void NativePopupEventFilter::append(QWindow& nativePopup)
{
    nativePopups_.push_back(&nativePopup);
}

bool NativePopupEventFilter::remove(std::uint32_t index, std::uint32_t removeCount)
{
    auto count = this->count();
    if(index < count && index + removeCount <= count)
    {
        nativePopups_.erase(
            nativePopups_.begin() + index,
            nativePopups_.begin() + index + removeCount
        );
        return true;
    }
    return false;
}

void NativePopupEventFilter::clear()
{
    nativePopups_.clear();
}

bool NativePopupEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == parentWindow_)
    {
        auto type = event->type();
        if(type == QEvent::Type::MouseMove
            || type == QEvent::Type::MouseButtonPress
            || type == QEvent::Type::MouseButtonRelease
            || type == QEvent::Type::MouseButtonDblClick
            || type == QEvent::Type::Gesture
            || type == QEvent::Type::GestureOverride
            || type == QEvent::Type::NativeGesture
            || type == QEvent::Type::KeyRelease
        )
        {
            for(auto* nativePopup: nativePopups_)
            {
                static_cast<QObject*>(nativePopup)->event(event);
            }
            return true;
        }
        else if(type == QEvent::Type::KeyPress)
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);
            // Windows: Alt + F4; macOS: Command + Q
            if(keyEvent->matches(QKeySequence::StandardKey::Close))
            {
                for(auto* nativePopup: nativePopups_)
                {
                    nativePopup->close();
                }
                parentWindow_->close();
                return true;
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

bool NativePopupEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
    return false;
}
}
