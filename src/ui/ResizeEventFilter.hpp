#ifndef YADAW_SRC_UI_RESIZEEVENTFILTER
#define YADAW_SRC_UI_RESIZEEVENTFILTER

#include "WindowAndId.hpp"

#include <QAbstractNativeEventFilter>
#include <QObject>

namespace YADAW::UI
{
class ResizeEventFilter: public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    enum DragPosition: std::uint8_t
    {
        TopLeft,
        Top,
        TopRight,
        Left,
        Right,
        BottomLeft,
        Bottom,
        BottomRight
    };
    enum FeatureSupportFlag: std::uint64_t
    {
        SupportsStartStopResize       = 1 << 0,
        SupportsAdjustOnAboutToResize = 1 << 1
    };
    using FeatureSupportFlags = std::underlying_type_t<FeatureSupportFlag>;
public:
    ResizeEventFilter(QWindow& window);
    ~ResizeEventFilter() override;
public:
    static constexpr FeatureSupportFlags getNativeSupportFlags();
signals:
    void startResize(); // needs SupportsStartStopResize
    void aboutToResize(DragPosition dragPosition, QRect* rect); // needs SupportsAdjustOnAboutToResize
    void resized(QRect rect);
    void endResize(); // needs SupportsStartStopResize
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    WindowAndId windowAndId_;
    DragPosition position_;
    bool aboutToStartResize_ = false;
    bool resizing_ = false;
#if _WIN32
    bool prevIsCaptureChanged_ = false;
#endif
};
}

#endif // YADAW_SRC_UI_RESIZEEVENTFILTER