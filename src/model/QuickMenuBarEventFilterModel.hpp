#ifndef YADAW_SRC_MODEL_QUICKMENUBAREVENTFILTERMODEL
#define YADAW_SRC_MODEL_QUICKMENUBAREVENTFILTERMODEL

#include "model/INativePopupEventFilterModel.hpp"
#include "ui/QuickMenuBarEventFilter.hpp"

#include <QWindow>

namespace YADAW::Model
{
class QuickMenuBarEventFilterModel: public INativePopupEventFilterModel
{
    Q_OBJECT
private:
    QuickMenuBarEventFilterModel(QWindow& parentWindow, QObject& menuBar);
public:
    static QuickMenuBarEventFilterModel* create(QWindow& parentWindow, QObject& menuBar);
    ~QuickMenuBarEventFilterModel() override;
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
    YADAW::UI::QuickMenuBarEventFilter quickMenuBarEventFilter_;
};
}

#endif // YADAW_SRC_MODEL_QUICKMENUBAREVENTFILTERMODEL