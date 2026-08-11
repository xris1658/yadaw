#include "Plugin.hpp"

#include <private/qsgdefaultcontext_p.h>
#include <private/qsgrhisupport_p.h>
#include <private/qsgthreadedrenderloop_p.h>

D3D11TSRTAdaptationPlugin::D3D11TSRTAdaptationPlugin(QObject* parent):
    QSGContextPlugin(parent)
{}

QStringList D3D11TSRTAdaptationPlugin::keys() const
{
    return QStringList {"d3d11-tsrt-adaptation"};
}

QSGContext* D3D11TSRTAdaptationPlugin::create(const QString& key) const
{
    return new QSGDefaultContext();
}

QSGRenderLoop* D3D11TSRTAdaptationPlugin::createWindowManager()
{
// #if QT_CONFIG(thread)
//     return new QSGThreadedRenderLoop();
// #else
    return nullptr; // FIXME: resolve undefined reference to ctor of `QSGThreadedRenderLoop`
// #endif
}

QSGContextFactoryInterface::Flags D3D11TSRTAdaptationPlugin::flags(const QString &key) const
{
    return {};
}