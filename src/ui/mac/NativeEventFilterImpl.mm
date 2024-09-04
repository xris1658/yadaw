#if __APPLE__

#import <AppKit/NSEvent.h>

#include <QByteArray>

#include <cstdio>
#include <map>

namespace YADAW::UI
{
std::map<int, const char*> eventNames = {
    {1,  "NSEventTypeLeftMouseDown"},
    {2,  "NSEventTypeLeftMouseUp"},
    {3,  "NSEventTypeRightMouseDown"},
    {4,  "NSEventTypeRightMouseUp"},
    {5,  "NSEventTypeMouseMoved"},
    {6,  "NSEventTypeLeftMouseDragged"},
    {7,  "NSEventTypeRightMouseDragged"},
    {8,  "NSEventTypeMouseEntered"},
    {9,  "NSEventTypeMouseExited"},
    {10, "NSEventTypeKeyDown"},
    {11, "NSEventTypeKeyUp"},
    {12, "NSEventTypeFlagsChanged"},
    {13, "NSEventTypeAppKitDefined"},
    {14, "NSEventTypeSystemDefined"},
    {15, "NSEventTypeApplicationDefined"},
    {16, "NSEventTypePeriodic"},
    {17, "NSEventTypeCursorUpdate"},
    {18, "NSEventTypeRotate"},
    {19, "NSEventTypeBeginGesture"},
    {20, "NSEventTypeEndGesture"},
    {22, "NSEventTypeScrollWheel"},
    {23, "NSEventTypeTabletPoint"},
    {24, "NSEventTypeTabletProximity"},
    {25, "NSEventTypeOtherMouseDown"},
    {26, "NSEventTypeOtherMouseUp"},
    {27, "NSEventTypeOtherMouseDragged"},
    {29, "NSEventTypeGesture"},
    {30, "NSEventTypeMagnify"},
    {31, "NSEventTypeSwipe"},
    {32, "NSEventTypeSmartMagnify"},
    {33, "NSEventTypeQuickLook"},
    {34, "NSEventTypePressure"},
    {37, "NSEventTypeDirectTouch"},
    {38, "NSEventTypeChangeMode"}
};

std::map<int, const char*> appKitEventNames = {
    {0, "NSEventSubtypeWindowExposed"},
    {1, "NSEventSubtypeApplicationActivated"},
    {2, "NSEventSubtypeApplicationDeactivated"},
    {4, "NSEventSubtypeWindowMoved"},
    {8, "NSEventSubtypeScreenChanged"}
};

bool nativePopupNativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
    if(eventType == "mac_generic_NSEvent")
    {
        auto event = static_cast<NSEvent*>(message);
        auto type = static_cast<int>([event type]);
        auto it = eventNames.find(type);
        const char* name = it != eventNames.end()? it->second: "Unknown";
        std::printf("Event type: %lu\t%s\n",
            static_cast<unsigned long>([event type]),
            name
        );
        if(type == NSEventTypeAppKitDefined)
        {
            auto subType = [event subtype];
            auto it = appKitEventNames.find(subType);
            const char* name = it != appKitEventNames.end()? it->second: "Unknown";
            std::printf("  Subtype: %lu\t%s\n",
                static_cast<unsigned long>([event subtype]),
                name
            );
        }
    }
    return false;
}
}

#endif