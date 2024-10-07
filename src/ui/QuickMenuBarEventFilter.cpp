#include "QuickMenuBarEventFilter.hpp"

#include "util/Base.hpp"
#include "util/IntegerRange.hpp"

#include <QKeySequence>
#include <QQuickItem>
#include <QQuickWindow>

#if _WIN32
#include <Windows.h>
#elif __linux__
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

#include <cinttypes>
#include <string_view>

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
QuickMenuBarEventFilter::QuickMenuBarEventFilter(
    QWindow& parentWindow, QObject& menuBar):
    QObject(nullptr),
    QAbstractNativeEventFilter(),
    parentWindow_(parentWindow),
    menuBar_(&menuBar)
{
    parentWindow_.window->installEventFilter(this);
    QCoreApplication::instance()->installNativeEventFilter(this);
    QObject::connect(
        menuBar_, SIGNAL(countChanged()),
        this, SLOT(menuBarCountChanged())
    );
    installMenuBarItemEventFilter();
}

QuickMenuBarEventFilter::~QuickMenuBarEventFilter()
{
    QCoreApplication::instance()->removeNativeEventFilter(this);
    parentWindow_.window->removeEventFilter(this);
}

const QWindow& QuickMenuBarEventFilter::parentWindow() const
{
    return *parentWindow_.window;
}

QWindow& QuickMenuBarEventFilter::parentWindow()
{
    return *parentWindow_.window;
}

bool QuickMenuBarEventFilter::empty() const
{
    return nativePopups_.empty();
}

std::uint32_t QuickMenuBarEventFilter::count() const
{
    return nativePopups_.size();
}

const QWindow& QuickMenuBarEventFilter::operator[](std::uint32_t index) const
{
    return *nativePopups_[index].window;
}

QWindow& QuickMenuBarEventFilter::operator[](std::uint32_t index)
{
    return *nativePopups_[index].window;
}

const QWindow& QuickMenuBarEventFilter::at(std::uint32_t index) const
{
    return *nativePopups_.at(index).window;
}

QWindow& QuickMenuBarEventFilter::at(std::uint32_t index)
{
    return *nativePopups_.at(index).window;
}

bool QuickMenuBarEventFilter::insert(QWindow& nativePopup, std::uint32_t index, bool shouldReceiveKeyEvents)
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

void QuickMenuBarEventFilter::append(QWindow& nativePopup, bool shouldReceiveKeyEvents)
{
    nativePopups_.emplace_back(nativePopup);
    shouldReceiveKeyEvents_.emplace_back(shouldReceiveKeyEvents);
}

void QuickMenuBarEventFilter::popupShouldReceiveKeyEvents(std::uint32_t index, bool shouldReceiveKeyEvents)
{
    if(index < count())
    {
        shouldReceiveKeyEvents_[index] = shouldReceiveKeyEvents;
    }
}

bool QuickMenuBarEventFilter::remove(std::uint32_t index, std::uint32_t removeCount)
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

void QuickMenuBarEventFilter::clear()
{
    for(auto& [window, winId]: nativePopups_)
    {
        window->close();
    }
    shouldReceiveKeyEvents_.clear();
    nativePopups_.clear();
}

static const char menuBarItemClassPrefix[] = "MenuBarItem_QMLTYPE_";

void QuickMenuBarEventFilter::menuBarCountChanged()
{
    auto itemCount = menuBar_->property("count").value<int>();
    if(itemCount > menuBarItems_.size())
    {
        if(auto menuBarAsQuickItem = qobject_cast<QQuickItem*>(menuBar_))
        {
            std::string_view menuBarItemNameView(
                menuBarItemClassPrefix,
                YADAW::Util::stringLength(menuBarItemClassPrefix)
            );
            auto childItems = menuBarAsQuickItem
                ->childItems().front() // QQuickRow
                ->childItems(); // `MenuBarItem`s
            for(auto childItem: childItems)
            {
                auto metaObject = childItem->metaObject();
                auto className = metaObject->className();
                std::string_view classNameView(className);
                if(
                    classNameView.size() >= menuBarItemNameView.size() &&
                    std::equal(
                        menuBarItemNameView.begin(), menuBarItemNameView.end(),
                        classNameView.begin()
                    )
                    &&
#if __cplusplus >= 202002L
                    !menuBarItems_.contains(childItem)
#else
                    menuBarItems_.find(childItem) == menuBarItems_.end()
#endif
                )
                {
                    childItem->installEventFilter(this);
                    menuBarItems_.emplace(childItem);
                    QObject::connect(
                        childItem, &QObject::destroyed,
                        [childItem, this]
                        {
                            childItem->removeEventFilter(this);
                            menuBarItems_.erase(childItem);
                        }
                    );
                }
            }
        }
    }
}

void QuickMenuBarEventFilter::installMenuBarItemEventFilter()
{
    if(auto menuBarAsQuickItem = qobject_cast<QQuickItem*>(menuBar_))
    {
        std::string_view menuBarItemNameView(
            menuBarItemClassPrefix,
            YADAW::Util::stringLength(menuBarItemClassPrefix)
        );
        auto childItems = menuBarAsQuickItem
            ->childItems().front() // QQuickRow
            ->childItems(); // `MenuBarItem`s
        for(auto childItem: childItems)
        {
            auto metaObject = childItem->metaObject();
            auto className = metaObject->className();
            std::string_view classNameView(className);
            if(
                classNameView.size() >= menuBarItemNameView.size() &&
                std::equal(
                    menuBarItemNameView.begin(), menuBarItemNameView.end(),
                    classNameView.begin()
                )
            )
            {
                childItem->installEventFilter(this);
                menuBarItems_.emplace(childItem);
                QObject::connect(
                    childItem, &QObject::destroyed,
                    [childItem, this]
                    {
                        childItem->removeEventFilter(this);
                        menuBarItems_.erase(childItem);
                    }
                );
            }
        }
    }
}

static const char menuItemClassPrefix[] = "MenuItem_QMLTYPE_";

class BoolFlag
{
public:
    BoolFlag(bool& flag): flag_(flag) { flag_ = true; }
    ~BoolFlag() { flag_ = false; }
private:
    bool& flag_;
};

bool QuickMenuBarEventFilter::eventFilter(QObject* watched, QEvent* event)
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
            }
            return watched->event(event);
        }
        else if(type == QEvent::Type::Gesture
            || type == QEvent::Type::GestureOverride
            || type == QEvent::Type::NativeGesture
            || type == QEvent::Type::TouchBegin
            || type == QEvent::Type::TouchCancel
            || type == QEvent::Type::TouchEnd
            || type == QEvent::Type::TouchUpdate)
        {
            auto ret = false;
            for(auto& [nativePopup, winId]: nativePopups_)
            {
                ret |= static_cast<QObject*>(nativePopup)->event(event);
            }
            return ret | watched->event(event);
        }
        if(type == QEvent::Type::KeyPress
            || type == QEvent::Type::KeyRelease)
        {
            if(!nativePopups_.empty())
            {
                auto ret = false;
                FOR_RANGE0(i, nativePopups_.size())
                {
                    auto& nativePopup = nativePopups_[i].window;
                    auto shouldSendKeyPressed = shouldReceiveKeyEvents_[i];
                    auto nativePopupAsQuickWindow = qobject_cast<QQuickWindow*>(nativePopup);
                    if(nativePopupAsQuickWindow && shouldSendKeyPressed)
                    {
                        // We reparented `Menu`s to a native popup in
                        // `MenuBarItem` and `MenuItem`:
                        // menu.parent = nativePopup.contentItem;
                        auto receiveKeyEventItem =
                            nativePopupAsQuickWindow->contentItem() // nativePopup.contentItem
                            ->childItems().front() // QQuickOverlay
                            ->childItems().front(); // QQuickPopupItem
                        auto keyEvent = static_cast<QKeyEvent*>(event);
                        // Menu mnemonic keys
                        if((type == QEvent::Type::KeyPress) &&
                            (keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z)
                            || (keyEvent->key() >= Qt::Key_0 && keyEvent->key() <= Qt::Key_9))
                        {
                            event->accept();
                            auto listView = receiveKeyEventItem // QQuickPopupItem
                                ->childItems().front(); // QQuickListView (Menu.contentItem)
                            auto count = listView->property("count").value<int>();
                            auto listViewMetaObject = listView->metaObject();
                            auto methodIndex = listViewMetaObject->indexOfMethod("itemAtIndex(int)");
                            if(methodIndex != -1)
                            {
                                auto itemAtIndex = [
                                    listView,
                                    method = listViewMetaObject->method(methodIndex)
                                ](int index) -> QQuickItem*
                                {
                                    QQuickItem* ret = nullptr;
                                    method.invoke(listView, qReturnArg(ret), index);
                                    return ret;
                                };
                                std::string_view stringViewMenuItemClassPrefix(
                                    menuItemClassPrefix,
                                    YADAW::Util::stringLength(menuItemClassPrefix)
                                );
                                std::vector<std::uint32_t> matchedItemIndices;
                                FOR_RANGE0(j, count)
                                {
                                    if(auto item = itemAtIndex(j))
                                    {
                                        if(
                                            std::string_view stringViewClassName(item->metaObject()->className());
                                            stringViewClassName.size() >= stringViewMenuItemClassPrefix.size() &&
                                            std::equal(
                                                stringViewMenuItemClassPrefix.begin(), stringViewMenuItemClassPrefix.end(),
                                                stringViewClassName.begin()
                                            )
                                        )
                                        {
                                            auto text = item->property("text").value<QString>();
                                            auto keySeq = QKeySequence::mnemonic(text);
                                            if(!keySeq.isEmpty())
                                            {
                                                auto key = YADAW::Util::underlyingValue(keySeq[0].key());
                                                if(key == keyEvent->key())
                                                {
                                                    matchedItemIndices.emplace_back(j);
                                                }
                                            }
                                        }
                                    }
                                }
                                auto size = matchedItemIndices.size();
                                if(size >= 1)
                                {
                                    auto currentIndex = listView->property("currentIndex").value<int>();
                                    auto it = std::lower_bound(matchedItemIndices.begin(), matchedItemIndices.end(), currentIndex);
                                    if(it == matchedItemIndices.end())
                                    {
                                        it = matchedItemIndices.begin();
                                    }
                                    // Parent object of `NativePopup` (see Menu.qml)
                                    if(auto menu = static_cast<QObject*>(nativePopup)->parent())
                                    {
                                        menu->setProperty("currentIndex", QVariant::fromValue<int>(*it));
                                    }
                                }
                                if(size == 1)
                                {
                                    if(auto selectedItem = itemAtIndex(matchedItemIndices.front()))
                                    {
                                        QShortcutEvent shortcutEvent(
                                            QKeySequence(
                                                QKeyCombination(
                                                    Qt::Modifier::ALT,
                                                    static_cast<Qt::Key>(keyEvent->key())
                                                )
                                            )
                                            );
                                        auto metaObject = selectedItem->metaObject();
                                        auto triggeredIndex = metaObject->indexOfMethod("clicked()");
                                        if(triggeredIndex != -1)
                                        {
                                            metaObject->method(triggeredIndex).invoke(selectedItem);
                                        }
                                        if(auto action = selectedItem->property("action").value<QObject*>())
                                        {
                                            auto metaObject = action->metaObject();
                                            if(
                                                auto triggeredIndex = metaObject->indexOfMethod("triggered()");
                                                triggeredIndex != -1
                                            )
                                            {
                                                metaObject->method(triggeredIndex).invoke(action);
                                            }
                                        }
                                    }
                                }
                            }
                            ret = true;
                        }
                        else
                        {
                            QCoreApplication::sendEvent(receiveKeyEventItem, event);
                            ret = event->isAccepted();
                        }
                        if(!ret)
                        {
                            auto key = keyEvent->key();
                            if(key == Qt::Key_Left || key == Qt::Key_Right)
                            {
                                event->accept();
                                QCoreApplication::sendEvent(menuBar_, event);
                            }
                        }
                    }
                }
                return ret;
            }
        }
    }
    else
    {
        if(auto quickItem = qobject_cast<QQuickItem*>(watched);
            quickItem &&
#if __cplusplus >= 202002L
            menuBarItems_.contains(quickItem)
#else
            menuBarItems_.find(quickItem) != menuBarItems_.end()
#endif
            && event->type() == QEvent::Shortcut
            && !filteringMenuBarEvents_
        )
        {
            BoolFlag flag(filteringMenuBarEvents_);
            if(QCoreApplication::sendEvent(watched, event))
            {
                if(auto menu = quickItem->property("menu").value<QObject*>())
                {
                    menu->setProperty("currentIndex", QVariant::fromValue<int>(0));
                }
                return true;
            }
        }
    }
    return false;
}

bool QuickMenuBarEventFilter::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
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
            if(!nativePopups_.empty())
            {
                auto metaObject = menuBar_->metaObject();
                auto signalIndex = metaObject->indexOfMethod("closeAllMenus()");
                if(signalIndex != -1)
                {
                    metaObject->method(signalIndex).invoke(menuBar_);
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
