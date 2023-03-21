#ifndef PLUGINWINDOWTHREAD
#define PLUGINWINDOWTHREAD

#include <QThread>
#include <QWindow>

class PluginWindowThread: public QThread
{
	Q_OBJECT
public:
	PluginWindowThread(QObject* parent = nullptr);
	virtual ~PluginWindowThread();
protected:
	void run() override;
public:
	QWindow* window();
private:
	QWindow window_;
};

#endif