#ifndef YADAW_SRC_MODEL_NATIVEPOPUPEVENTFILTERMODEL
#define YADAW_SRC_MODEL_NATIVEPOPUPEVENTFILTERMODEL

#include "model/INativePopupEventFilterModel.hpp"
#include "ui/NativePopupEventFilter.hpp"

#include <QWindow>

namespace YADAW::Model
{
class NativePopupEventFilterModel: public INativePopupEventFilterModel
{
    Q_OBJECT
private:
    NativePopupEventFilterModel(QWindow& parentWindow);
public:
    static NativePopupEventFilterModel* create(QWindow& parentWindow);
    ~NativePopupEventFilterModel() override;
public:
    std::uint32_t itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    public:
    bool insert(QWindow* nativePopup, int index, bool shouldReceiveKeyEvents) override;
    bool append(QWindow* nativePopup, bool shouldReceiveKeyEvents) override;
    void popupShouldReceiveKeyEvents(QWindow* nativePopup, bool shouldReceiveKeyEvents) override;
    bool remove(QWindow* nativePopup) override;
    void clear() override;
private:
    YADAW::UI::NativePopupEventFilter nativePopupEventFilter_;
};
}

#endif // YADAW_SRC_MODEL_NATIVEPOPUPEVENTFILTERMODEL
