#ifndef YADAW_CONTENT_MODEL_METERSCALEMODEL
#define YADAW_CONTENT_MODEL_METERSCALEMODEL

#include"ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class MeterScaleModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        DecibelStop = Qt::UserRole,
        PositionStop,
        ColorBelow,
        ColorAbove,
        RoleCount
    };
public:
    MeterScaleModel(QObject* parent = nullptr);
    ~MeterScaleModel();
public:
    int itemCount() const;
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override final;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    Q_INVOKABLE int add(double decibelStop, double positionStop, double colorBelow, double colorAbove);
    Q_INVOKABLE int remove(int position, int count);
    Q_INVOKABLE void clear();
protected:
    RoleNames roleNames() const override;
private:
    std::vector<double> decibelStops_;
    std::vector<double> positionStops_;
    std::vector<QColor> colorsBelow_;
    std::vector<QColor> colorsAbove_;
};
}

#endif // YADAW_CONTENT_MODEL_METERSCALEMODEL