#ifndef YADAW_TOOLS_D3D11_TSRT_ADAPTATION_PLUGIN
#define YADAW_TOOLS_D3D11_TSRT_ADAPTATION_PLUGIN

#include <private/qsgcontextplugin_p.h>

#include <QObject>

class D3D11TSRTAdaptationPlugin: public QSGContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSGContextFactoryInterface" FILE "d3d11-tsrt-adaptation.json")
public:
    D3D11TSRTAdaptationPlugin(QObject* parent = nullptr);
    QStringList keys() const;
    QSGContext* create(const QString& key) const;
    QSGRenderLoop* createWindowManager();
    Flags flags(const QString& key) const;
};

#endif // YADAW_TOOLS_D3D11_TSRT_ADAPTATION_PLUGIN