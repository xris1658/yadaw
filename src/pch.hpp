#ifndef YADAW_PCH
#define YADAW_PCH

// SQLite / sqlite_modern_cpp
#include <sqlite_modern_cpp.h>

// yaml-cpp
#include <yaml-cpp/yaml.h>

// CLAP
#include <clap/events.h>
#include <clap/host.h>
#include <clap/entry.h>
#include <clap/plugin.h>
#include <clap/plugin-features.h>
#include <clap/ext/audio-ports.h>
#include <clap/ext/params.h>
#include <clap/ext/gui.h>
#include <clap/factory/plugin-factory.h>

// VST3
#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstattributes.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivsthostapplication.h>
#include <pluginterfaces/vst/ivstmessage.h>
#include <pluginterfaces/vst/ivstnoteexpression.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <pluginterfaces/vst/ivstprocesscontext.h>
#include <pluginterfaces/vst/ivstrepresentation.h>
#include <pluginterfaces/vst/vstspeaker.h>

// Qt
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtGlobal>
#include <QString>
#include <QList>
#include <QWindow>
#include <QQuickWindow>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QtCore/qobjectdefs.h>
#include <QObject>
#include <QColor>
#include <qqml.h>
#include <QDebug>
#include <QQuickItem>
#include <QThread>
#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>
#include <QVector>
#include <QTranslator>
#include <QScreen>

// Windows API
#if(WIN32)
#include <Windows.h>
#include <winreg.h>
#include <winerror.h>
#include <errhandlingapi.h>
#include <libloaderapi.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <realtimeapiset.h>
#include <timezoneapi.h>
#include <combaseapi.h>

// C++/WinRT API
#include "native/winrt/Forward.hpp"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Midi.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Render.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>
#endif


// C/C++ header without templates
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <charconv>
#include <mutex>
#include <cwchar>

// C/C++ header with templates
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <utility>
#include <string>
#include <tuple>
#include <thread>
#include <future>
#include <bitset>
#include <set>
#include <map>
#include <variant>
#include <type_traits>
#include <system_error>
#include <list>
#include <functional>
#include <stack>
#include <queue>
#include <fstream>

#endif //YADAW_PCH
