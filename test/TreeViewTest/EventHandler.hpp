#ifndef YADAW_TEST_TREEVIEWTEST_EVENTHANDLER
#define YADAW_TEST_TREEVIEWTEST_EVENTHANDLER

#include "SimpleTreeModel.hpp"

#include <QObject>

class EventHandler: public QObject
{
    Q_OBJECT
public:
    EventHandler(SimpleTreeModel& model, QObject* parent = nullptr);
    ~EventHandler();
public slots:
    void onModifyModel();
private:
    SimpleTreeModel& stm_;
    int index_ = 0;
};

#endif // YADAW_TEST_TREEVIEWTEST_EVENTHANDLER