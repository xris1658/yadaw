#ifndef YADAW_SRC_MODEL_QUICKMENUBAREVENTFILTERMODEL
#define YADAW_SRC_MODEL_QUICKMENUBAREVENTFILTERMODEL

#include "model/INativePopupEventFilterModel.hpp"
#include "ui/QuickMenuEventFilter.hpp"

#include <QWindow>

namespace YADAW::Model
{
class QuickMenuEventFilterModel: public INativePopupEventFilterModel
{
    Q_OBJECT
private:
    QuickMenuEventFilterModel(QWindow& parentWindow);
    QuickMenuEventFilterModel(QWindow& parentWindow, QObject& menuBar);
public:
    static QuickMenuEventFilterModel* create(QWindow& parentWindow);
    static QuickMenuEventFilterModel* create(QWindow& parentWindow, QObject& menuBar);
    ~QuickMenuEventFilterModel() override;
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
    YADAW::UI::QuickMenuEventFilter quickMenuBarEventFilter_;
};
}

#endif // YADAW_SRC_MODEL_QUICKMENUBAREVENTFILTERMODEL