#include "NativePopupEventFilter.hpp"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QQuickItem>
#include <QQuickWindow>

#include "util/IntegerRange.hpp"
#include "ui/Runtime.hpp"

#if _WIN32
#include <Windows.h>
#elif __linux__
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

#include <cinttypes>

#if __linux__
static std::map<int, const char*> events = {
    {2, "KeyPress"},
    {3, "KeyRelease"},
    {4, "ButtonPress"},
    {5, "ButtonRelease"},
    {6, "MotionNotify"},
    {7, "EnterNotify"},
    {8, "LeaveNotify"},
    {9, "FocusIn"},
    {10, "FocusOut"},
    {11, "KeymapNotify"},
    {12, "Expose"},
    {13, "GraphicsExposure"},
    {14, "NoExposure"},
    {15, "VisibilityNotify"},
    {16, "CreateNotify"},
    {17, "DestroyNotify"},
    {18, "UnmapNotify"},
    {19, "MapNotify"},
    {20, "MapRequest"},
    {21, "ReparentNotify"},
    {22, "ConfigureNotify"},
    {23, "ConfigureRequest"},
    {24, "GravityNotify"},
    {25, "ResizeRequest"},
    {26, "CirculateNotify"},
    {27, "CirculateRequest"},
    {28, "PropertyNotify"},
    {29, "SelectionClear"},
    {30, "SelectionRequest"},
    {31, "SelectionNotify"},
    {32, "ColormapNotify"},
    {33, "ClientMessage"},
    {34, "MappingNotify"},
    {35, "GeGeneric"}
};
#endif

namespace YADAW::UI
{
NativePopupEventFilter::NativePopupEventFilter(QWindow& parentWindow):
    QObject(nullptr),
    QAbstractNativeEventFilter(),
    parentWindow_(parentWindow)
{
    parentWindow.installEventFilter(this);
    QCoreApplication::instance()->installNativeEventFilter(this);
}

NativePopupEventFilter::~NativePopupEventFilter()
{
    QCoreApplication::instance()->removeNativeEventFilter(this);
    parentWindow_.window->removeEventFilter(this);
}

const QWindow& NativePopupEventFilter::parentWindow() const
{
    return *parentWindow_.window;
}

QWindow& NativePopupEventFilter::parentWindow()
{
    return *parentWindow_.window;
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
    return *nativePopups_[index].window;
}

QWindow& NativePopupEventFilter::operator[](std::uint32_t index)
{
    return *nativePopups_[index].window;
}

const QWindow& NativePopupEventFilter::at(std::uint32_t index) const
{
    return *nativePopups_.at(index).window;
}

QWindow& NativePopupEventFilter::at(std::uint32_t index)
{
    return *nativePopups_.at(index).window;
}

bool NativePopupEventFilter::insert(QWindow& nativePopup, std::uint32_t index,
    bool shouldReceiveKeyEvents)
{
    if(index < count())
    {
        nativePopups_.emplace(nativePopups_.begin() + index, nativePopup);
        shouldReceiveKeyEvents_.emplace(
            shouldReceiveKeyEvents_.begin() + index, shouldReceiveKeyEvents
        );
        return true;
    }
    return false;
}

void NativePopupEventFilter::append(QWindow& nativePopup, bool shouldReceiveKeyEvents)
{
    nativePopups_.emplace_back(nativePopup);
    shouldReceiveKeyEvents_.emplace_back(shouldReceiveKeyEvents);
}

void NativePopupEventFilter::popupShouldReceiveKeyEvents(std::uint32_t index, bool shouldReceiveKeyEvents)
{
    if(index < count())
    {
        shouldReceiveKeyEvents_[index] = shouldReceiveKeyEvents;
    }
}

bool NativePopupEventFilter::remove(std::uint32_t index, std::uint32_t removeCount)
{
    auto count = this->count();
    if(index < count && index + removeCount <= count)
    {
        FOR_RANGE(i, index, index + removeCount)
        {
            nativePopups_[i].window->close();
        }
        shouldReceiveKeyEvents_.erase(
            shouldReceiveKeyEvents_.begin() + index,
            shouldReceiveKeyEvents_.begin() + index + removeCount
        );
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
    for(auto& [window, winId]: nativePopups_)
    {
        window->close();
    }
    shouldReceiveKeyEvents_.clear();
    nativePopups_.clear();
}

bool NativePopupEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == parentWindow_.window)
    {
        auto type = event->type();
        // We don't need to handle mouse move events manually.
        if(type == QEvent::Type::MouseButtonPress
            || type == QEvent::Type::MouseButtonRelease
            || type == QEvent::Type::MouseButtonDblClick)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            auto globalPosition = mouseEvent->globalPosition();
            if(nativePopups_.empty())
            {
                return false;
            }
            auto onNativePopups = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                auto shouldSendMousePressed =
                       globalPosition.x() <  nativePopup->x()
                    || globalPosition.x() >= nativePopup->x() + nativePopup->width()
                    || globalPosition.y() <  nativePopup->y()
                    || globalPosition.y() >= nativePopup->y() + nativePopup->height();
                if(shouldSendMousePressed)
                {
                    auto metaObject = nativePopup->metaObject();
                    auto signalIndex = metaObject->indexOfSignal("mousePressedOutside()");
                    if(signalIndex != -1)
                    {
                        metaObject->method(signalIndex).invoke(nativePopup);
                    }
                }
                onNativePopups |= (!shouldSendMousePressed);
            }
            return onNativePopups;
        }
        else if(type == QEvent::Type::FocusOut)
        {
            auto focusEvent = static_cast<QFocusEvent*>(event);
            if(focusEvent->reason() != Qt::FocusReason::PopupFocusReason)
            {
                for(auto& [nativePopup, winId]: nativePopups_)
                {
                    auto metaObject = nativePopup->metaObject();
                    auto signalIndex = metaObject->indexOfSignal("mousePressedOutside()");
                    if(signalIndex != -1)
                    {
                        metaObject->method(signalIndex).invoke(nativePopup);
                    }
                    else
                    {
                        qDebug("Cannot find `mousePressedOutside()`");
                    }
                }
            }
        }
    }
    return false;
}

bool NativePopupEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
#if _WIN32
    if(eventType == "windows_generic_MSG")
    {
        auto msg = static_cast<MSG*>(message);
        // Mouse click in the parent window's non-client area, e.g. title bar
        // `WM_NCLBUTTONDOWN` starts dragging and moving the window;
        // `WM_NCRBUTTONUP` after `WM_NCRBUTTONDOWN` shows the control menu
        if(
            (msg->message == WM_NCLBUTTONDOWN || msg->message == WM_NCRBUTTONDOWN)
            && msg->hwnd == reinterpret_cast<HWND>(parentWindow_.winId)
        )
        {
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                auto metaObject = nativePopup->metaObject();
                auto signalIndex = metaObject->indexOfSignal("mousePressedOutside()");
                if(signalIndex != -1)
                {
                    metaObject->method(signalIndex).invoke(nativePopup);
                }
                else
                {
                    qDebug("Cannot find `mousePressedOutside()`");
                }
            }
            return false;
        }
    }
#elif __linux__
    if (eventType == "xcb_generic_event_t")
    {
        auto event = static_cast<xcb_generic_event_t*>(message);
        auto responseType = event->response_type & 0x7F;
        auto it = events.find(responseType);
        const char* eventTypeText = it != events.end()? it->second: "Unknown";
#if PRINT_NATIVE_EVENTS
        qDebug("%" PRIu32", %" PRIu8": %s", event->full_sequence, responseType, eventTypeText);
#endif
        if(responseType == XCB_PROPERTY_NOTIFY)
        {
            auto propertyNotifyEvent = reinterpret_cast<xcb_property_notify_event_t*>(event);
            // propertyNotifyEvent->
        }
        if(responseType == XCB_CLIENT_MESSAGE)
        {
            auto clientMessageEvent = reinterpret_cast<xcb_client_message_event_t*>(event);
            auto x11Interface = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
            auto connection = x11Interface->connection();
            auto windowHandle = static_cast<xcb_window_t>(parentWindow_.winId);
            auto getAtomNameCookie = xcb_get_atom_name(connection, clientMessageEvent->type);
            xcb_generic_error_t* error = nullptr;
            auto reply = xcb_get_atom_name_reply(connection, getAtomNameCookie, &error);
            if(!error)
            {
                std::vector<char> name(xcb_get_atom_name_name_length(reply) + 1);
                name[name.size() - 1] = 0;
                std::memcpy(name.data(), xcb_get_atom_name_name(reply), name.size() - 1);
#if PRINT_NATIVE_EVENTS
                std::fprintf(stderr, "Atom: 0x%" PRIX32" (%s)\n", clientMessageEvent->type, name.data());
#endif
                if(std::strcmp(name.data(), "WM_PROTOCOLS") == 0)
                {
                    getAtomNameCookie = xcb_get_atom_name(connection, clientMessageEvent->data.data32[0]);
                    reply = xcb_get_atom_name_reply(connection, getAtomNameCookie, &error);
                    if(!error)
                    {
                        name.resize(xcb_get_atom_name_name_length(reply) + 1);
                        name[name.size() - 1] = 0;
                        std::memcpy(name.data(), xcb_get_atom_name_name(reply), name.size() - 1);
#if PRINT_NATIVE_EVENTS
                        std::fprintf(stderr, "Client message atom: 0x%" PRIX32" (%s)\n", clientMessageEvent->data.data32[0], name.data());
#endif
                        if(std::strcmp(name.data(), "WM_TAKE_FOCUS") == 0)
                        {
                            for(auto& [nativePopup, winId]: nativePopups_)
                            {
                                auto metaObject = nativePopup->metaObject();
                                auto signalIndex = metaObject->indexOfSignal("mousePressedOutside()");
                                if(signalIndex != -1)
                                {
                                    metaObject->method(signalIndex).invoke(nativePopup);
                                }
                            }
                        }
                    }
#if PRINT_NATIVE_EVENTS
                    else
                    {
                        std::fprintf(stderr, "Client message atom: 0x%" PRIX32"\n", clientMessageEvent->data.data32[0]);
                    }
#endif
                }
            }
#if PRINT_NATIVE_EVENTS
            else
            {
                std::fprintf(stderr, "Atom: 0x%" PRIX32"\n", clientMessageEvent->type);
            }
#endif
            auto& clientMessageData = clientMessageEvent->data;
#if PRINT_NATIVE_EVENTS
            const char c[2] = {' ', '\n'};
            std::fprintf(stderr, "    Data: ");
            switch(clientMessageEvent->format)
            {
            case 8:
            {
                FOR_RANGE0(i, std::size(clientMessageData.data8))
                {
                    std::fprintf(
                        stderr, "0x%" PRIX8"%c",
                        clientMessageData.data8[i],
                        c[i == std::size(clientMessageData.data8) - 1]);
                }
                break;
            }
            case 16:
            {
                FOR_RANGE0(i, std::size(clientMessageData.data16))
                {
                    std::fprintf(
                        stderr, "0x%" PRIX16"%c",
                        clientMessageData.data16[i],
                        c[i == std::size(clientMessageData.data16) - 1]);
                }
                break;
            }
            case 32:
            {
                FOR_RANGE0(i, std::size(clientMessageData.data32))
                {
                    std::fprintf(
                        stderr, "0x%" PRIX32"%c",
                        clientMessageData.data32[i],
                        c[i == std::size(clientMessageData.data32) - 1]);
                }
                break;
            }
            }
#endif
        }
    }
#endif
    return false;
}
}
