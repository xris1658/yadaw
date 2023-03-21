#include "PluginWindowThread.hpp"

PluginWindowThread::PluginWindowThread(QObject* parent): window_(static_cast<QWindow*>(nullptr))
{
	//
}

PluginWindowThread::~PluginWindowThread()
{
	//
}

void PluginWindowThread::run()
{
}

QWindow* PluginWindowThread::window()
{
	return &window_;
}

