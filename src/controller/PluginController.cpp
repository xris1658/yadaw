#include "PluginController.hpp"

#include "audio/host/VestifalCallback.hpp"
#include "audio/util/CLAPHelper.hpp"
#include "audio/util/VST3Helper.hpp"
#include "audio/util/VestifalHelper.hpp"
#include "dao/PluginCategoryTable.hpp"
#include "dao/PluginTable.hpp"
#include "native/Library.hpp"
#include "native/VST3Native.hpp"
#include "native/VestifalNative.hpp"
#include "util/IntegerRange.hpp"

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

#include <clap/plugin-features.h>

#include <QDir>
#include <QStringList>

#include <iterator>

#include <cstring>

namespace YADAW::Controller
{
using namespace YADAW::Audio::Device;
using namespace YADAW::Audio::Plugin;
using namespace YADAW::Native;

QLatin1StringView vst3Ext(YADAW::Native::vst3FilePattern + 2);
QLatin1StringView clapExt("clap");
QString vst3Pattern(YADAW::Native::vst3FilePattern);
QString clapPattern("*.clap");

std::vector<QString> scanDirectory(const QDir& dir, bool recursive, bool includeSymLink)
{
    QStringList nameFilters({vst3Pattern, clapPattern, vestifalPattern});
    QDir::Filters filters = QDir::Filter::Files | QDir::Filter::Hidden;
    const auto& entryInfoList = dir.entryInfoList(nameFilters,
        includeSymLink? filters: filters | QDir::Filter::NoSymLinks);
    std::vector<QString> ret; ret.reserve(entryInfoList.size());
    for(auto& entryInfo: entryInfoList)
    {
        if(entryInfo.isSymLink() && entryInfo.exists())
        {
            QFileInfo fileInfo(entryInfo.symLinkTarget());
            if(fileInfo.exists())
            {
                auto suffix = fileInfo.suffix();
                if(suffix == vst3Ext || suffix == clapExt || suffix == vestifalExt)
                {
                    ret.emplace_back(fileInfo.absoluteFilePath());
                }
            }
        }
        else if(entryInfo.exists())
        {
            ret.emplace_back(entryInfo.absoluteFilePath());
        }
    }
#if __APPLE__
    const auto& dirEntryInfoList = dir.entryInfoList(
        nameFilters,
        QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::NoDotAndDotDot
    );
    for(auto& entryInfo: dirEntryInfoList)
    {
        ret.emplace_back(entryInfo.absoluteFilePath());
    }
#endif
    if(recursive)
    {
        const auto& dirList = dir.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::NoDotAndDotDot);
        for(auto& dirInfo: dirList)
        {
            if(dirInfo.isDir() && dirInfo.exists())
            {
                const auto& unlinkedDirInfo =
                    (dirInfo.isSymLink() && dirInfo.exists())?
                        QFileInfo(dirInfo.symLinkTarget()):
                        dirInfo;
                QDir subdir(unlinkedDirInfo.absoluteFilePath());
                auto result = scanDirectory(subdir, true, includeSymLink);
                for(const auto& item: result)
                {
                    ret.emplace_back(std::move(item));
                }
            }
        }
    }
    return ret;
}

std::vector<PluginScanResult> scanSingleLibraryFile(const QString& path)
{
    Library library(path);
    if(!library.loaded())
    {
        return {};
    }
    if(path.endsWith(vst3Ext, Qt::CaseSensitivity::CaseInsensitive))
    {
        auto plugin = YADAW::Audio::Util::createVST3FromLibrary(library);
        if(auto factory = plugin.factory())
        {
            Steinberg::PFactoryInfo factoryInfo {};
            std::vector<PluginScanResult> ret;
            Steinberg::IPluginFactory2* factory2 = nullptr;
            Steinberg::IPluginFactory3* factory3 = nullptr;
            queryInterface(factory, &factory2);
            queryInterface(factory, &factory3);
            if(factory3)
            {
                auto classCount = factory3->countClasses();
                Steinberg::PClassInfoW classInfoW {};
                FOR_RANGE0(i, classCount)
                {
                    bool isInstrument = false;
                    bool isAudioEffect = false;
                    std::memset(classInfoW.subCategories, 0, std::size(classInfoW.subCategories));
                    if(factory3->getClassInfoUnicode(i, &classInfoW) == Steinberg::kResultOk
                    && std::strcmp(classInfoW.category, kVstAudioEffectClass) == 0)
                    {
                        char* subCategoryCollection[std::size(classInfoW.subCategories)];
                        auto subCategoryCount = YADAW::Audio::Util::splitSubCategories(
                            classInfoW.subCategories, subCategoryCollection);
                        std::vector<QString> subCategories;
                        subCategories.reserve(subCategoryCount);
                        for(int j = 0; j < subCategoryCount; ++j)
                        {
                            using namespace Steinberg::Vst::PlugType;
                            if(std::strcmp(subCategoryCollection[j], kInstrument) == 0)
                            {
                                isInstrument = true;
                            }
                            else if(std::strcmp(subCategoryCollection[j], kFx) == 0)
                            {
                                isAudioEffect = true;
                            }
                            else
                            {
                                subCategories.emplace_back(subCategoryCollection[j]);
                            }
                        }
                        std::vector<char> uid(16, 0);
                        std::memcpy(uid.data(), classInfoW.cid, 16);
                        auto name = QString::fromUtf16(classInfoW.name);
                        QString vendor;
                        if(classInfoW.vendor[0] != 0)
                        {
                            vendor = QString::fromUtf16(classInfoW.vendor);
                        }
                        else if(factory3->getFactoryInfo(&factoryInfo) == Steinberg::kResultOk)
                        {
                            factory3->getFactoryInfo(&factoryInfo);
                            vendor = factoryInfo.vendor;
                        }
                        auto version = QString::fromUtf16(classInfoW.version);
                        YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                            YADAW::DAO::PluginFormatVST3, -1);
                        if(isInstrument)
                        {
                            pluginInfo.type = YADAW::DAO::PluginTypeInstrument;
                            ret.push_back({pluginInfo, subCategories});
                        }
                        if(isAudioEffect)
                        {
                            pluginInfo.type = YADAW::DAO::PluginTypeAudioEffect;
                            ret.push_back({pluginInfo, subCategories});
                        }
                        else if(!isInstrument)
                        {
                            pluginInfo.type = YADAW::DAO::PluginTypeUnknown;
                            ret.push_back({pluginInfo, subCategories});
                        }
                    }
                }
            }
            else if(factory2)
            {
                auto classCount = factory2->countClasses();
                Steinberg::PClassInfo2 classInfo2 {};
                FOR_RANGE0(i, classCount)
                {
                    bool isInstrument = false;
                    bool isAudioEffect = false;
                    std::memset(classInfo2.subCategories, 0, std::size(classInfo2.subCategories));
                    if(factory2->getClassInfo2(i, &classInfo2) == Steinberg::kResultOk
                    && std::strcmp(classInfo2.category, kVstAudioEffectClass) == 0)
                    {
                        char* subCategoryCollection[std::size(classInfo2.subCategories)];
                        auto subCategoryCount = YADAW::Audio::Util::splitSubCategories(
                            classInfo2.subCategories, subCategoryCollection);
                        std::vector<QString> subCategories;
                        subCategories.reserve(subCategoryCount);
                        for(int j = 0; j < subCategoryCount; ++j)
                        {
                            using namespace Steinberg::Vst::PlugType;
                            if(std::strcmp(subCategoryCollection[j], kInstrument) == 0)
                            {
                                isInstrument = true;
                            }
                            else if(std::strcmp(subCategoryCollection[j], kFx) == 0)
                            {
                                isAudioEffect = true;
                            }
                            else
                            {
                                subCategories.emplace_back(subCategoryCollection[j]);
                            }
                        }
                        std::vector<char> uid(16, 0);
                        std::memcpy(uid.data(), classInfo2.cid, 16);
                        QString name(classInfo2.name);
                        QString vendor;
                        if(classInfo2.vendor[0] != 0)
                        {
                            factory2->getFactoryInfo(&factoryInfo);
                            vendor = factoryInfo.vendor;
                        }
                        else if(factory2->getFactoryInfo(&factoryInfo) == Steinberg::kResultOk)
                        {
                            vendor = classInfo2.vendor;
                        }
                        QString version(classInfo2.version);
                        YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                            YADAW::DAO::PluginFormatVST3, -1);
                        if(isInstrument)
                        {
                            pluginInfo.type = YADAW::DAO::PluginTypeInstrument;
                            ret.push_back({pluginInfo, subCategories});
                        }
                        if(isAudioEffect)
                        {
                            pluginInfo.type = YADAW::DAO::PluginTypeAudioEffect;
                            ret.push_back({pluginInfo, subCategories});
                        }
                        else if(!isInstrument)
                        {
                            pluginInfo.type = YADAW::DAO::PluginTypeUnknown;
                            ret.push_back({pluginInfo, subCategories});
                        }
                    }
                }
            }
            else
            {
                auto classCount = factory->countClasses();
                Steinberg::PClassInfo classInfo {};
                FOR_RANGE0(i, classCount)
                {
                    if(factory->getClassInfo(i, &classInfo) == Steinberg::kResultOk
                    && std::strcmp(classInfo.category, kVstAudioEffectClass) == 0)
                    {
                        if(plugin.createPlugin(classInfo.cid)
                        && plugin.initialize(44100, 1024))
                        {
                            std::vector<char> uid(16, 0);
                            std::memcpy(uid.data(), classInfo.cid, 16);
                            QString name(classInfo.name);
                            using namespace Steinberg::Vst;
                            auto component = plugin.component();
                            int busCount[4] = {0, 0, 0, 0};
                            auto& [ai, ao, ei, eo] = busCount;
                            ai = component->getBusCount(
                                MediaTypes::kAudio, BusDirections::kInput);
                            ao = component->getBusCount(
                                MediaTypes::kAudio, BusDirections::kOutput);
                            ei = component->getBusCount(
                                MediaTypes::kEvent, BusDirections::kInput);
                            eo = component->getBusCount(
                                MediaTypes::kEvent, BusDirections::kOutput);
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, {}, {},
                                YADAW::DAO::PluginFormatVST3, -1);
                            if(ei && ao)
                            {
                                if(ai)
                                {
                                    BusInfo busInfo {};
                                    bool hasMainAudioInput = false;
                                    bool hasMainEventInput = false;
                                    for(int j = 0; j < ai; ++j)
                                    {
                                        component->getBusInfo(MediaTypes::kAudio,
                                            BusDirections::kInput, j, busInfo);
                                        hasMainAudioInput |= (busInfo.busType == BusTypes::kMain);
                                    }
                                    for(int j = 0; j < ei; ++j)
                                    {
                                        component->getBusInfo(MediaTypes::kEvent,
                                            BusDirections::kInput, j, busInfo);
                                        hasMainEventInput |= (busInfo.busType == BusTypes::kMain);
                                    }
                                    if(hasMainAudioInput)
                                    {
                                        pluginInfo.type = YADAW::DAO::PluginTypeAudioEffect;
                                        ret.push_back({pluginInfo, {}});
                                    }
                                    if(hasMainEventInput)
                                    {
                                        pluginInfo.type = YADAW::DAO::PluginTypeInstrument;
                                        ret.push_back({pluginInfo, {}});
                                    }
                                    else if(!hasMainAudioInput)
                                    {
                                        pluginInfo.type = YADAW::DAO::PluginTypeUnknown;
                                        ret.push_back({pluginInfo, {}});
                                    }
                                }
                            }
                            if(ai && ao)
                            {
                                pluginInfo.type = YADAW::DAO::PluginTypeAudioEffect;
                                ret.push_back({pluginInfo, {}});
                            }
                            if(ei && eo)
                            {
                                pluginInfo.type = YADAW::DAO::PluginTypeMIDIEffect;
                                ret.push_back({pluginInfo, {}});
                            }
                        }
                        plugin.uninitialize();
                    }
                }
            }
            if(factory3)
            {
                factory3->release();
            }
            if(factory2)
            {
                factory2->release();
            }
            return ret;
        }
    }
    else if(path.endsWith(clapExt, Qt::CaseSensitivity::CaseInsensitive))
    {
        auto plugin = YADAW::Audio::Util::createCLAPFromLibrary(library);
        if(plugin.entry())
        {
            std::vector<PluginScanResult> ret;
            auto factory = plugin.factory();
            auto count = factory->get_plugin_count(factory);
            FOR_RANGE0(i, count)
            {
                bool isInstrument = false;
                bool isAudioEffect = false;
                bool isMIDIEffect = false;
                auto descriptor = factory->get_plugin_descriptor(factory, i);
                if(descriptor)
                {
                    auto features = descriptor->features;
                    std::vector<QString> featureCollection;
                    for(auto pFeature = features; *pFeature; ++pFeature)
                    {
                        auto feature = *pFeature;
                        if(std::strcmp(feature, CLAP_PLUGIN_FEATURE_INSTRUMENT) == 0)
                        {
                            isInstrument = true;
                        }
                        else if(std::strcmp(feature, CLAP_PLUGIN_FEATURE_AUDIO_EFFECT) == 0)
                        {
                            isAudioEffect = true;
                        }
                        else if(std::strcmp(feature, CLAP_PLUGIN_FEATURE_NOTE_EFFECT) == 0)
                        {
                            isMIDIEffect = true;
                        }
                        else
                        {
                            featureCollection.emplace_back(QString::fromUtf8(feature));
                            if(std::strcmp(feature, CLAP_PLUGIN_FEATURE_ANALYZER) == 0)
                            {
                                isAudioEffect = true;
                            }
                        }
                    }
                    std::vector<char> uid(std::strlen(descriptor->id) + 1, 0);
                    std::strcpy(uid.data(), descriptor->id);
                    auto name = QString::fromUtf8(descriptor->name);
                    auto vendor = QString::fromUtf8(descriptor->vendor);
                    auto version = QString::fromUtf8(descriptor->version);
                    YADAW::DAO::PluginInfo pluginInfo(
                        path, uid, name, vendor, version,
                        YADAW::DAO::PluginFormatCLAP, -1
                    );
                    if(isInstrument)
                    {
                        pluginInfo.type = YADAW::DAO::PluginTypeInstrument;
                        ret.push_back({pluginInfo, featureCollection});
                    }
                    if(isAudioEffect)
                    {
                        pluginInfo.type = YADAW::DAO::PluginTypeAudioEffect;
                        ret.push_back({pluginInfo, featureCollection});
                    }
                    if(isMIDIEffect)
                    {
                        pluginInfo.type = YADAW::DAO::PluginTypeMIDIEffect;
                        ret.push_back({pluginInfo, featureCollection});
                    }
                }
            }
            return ret;
        }
    }
    // Since Vestifal and VST3 plugin binaries share *.so extension on Linux and
    // macOS, we have to scan *.so files again if that is not a VST3 plugin.
#if _WIN32
    else
#endif
    if(path.endsWith(vestifalExt, Qt::CaseSensitivity::CaseInsensitive))
    {
        YADAW::Audio::Host::setUniquePluginShouldBeZeroOnCurrentThread(false);
        auto plugin = YADAW::Audio::Util::createVestifalFromLibrary(library);
        auto effect = plugin.effect();
        if(effect)
        {
            runDispatcher(effect, EffectOpcode::effectOpen);
            char name[128];
            char vendor[128];
            std::vector<char> uidAsVector(sizeof(std::uint32_t) + 1, 0);
            if(runDispatcher(effect, EffectOpcode::effectGetPlugCategory) == PluginCategory::PluginUnknown)
            {
                YADAW::Audio::Host::setUniquePluginShouldBeZeroOnCurrentThread(true);
                runDispatcher(effect, EffectOpcode::effectClose);
                plugin = {};
                plugin = YADAW::Audio::Util::createVestifalFromLibrary(library);
                effect = plugin.effect();
                runDispatcher(effect, EffectOpcode::effectOpen);
            }
            std::vector<std::int32_t> uids;
            if(runDispatcher(effect, EffectOpcode::effectGetPlugCategory) == PluginCategory::PluginShell)
            {
                char name[128];
                name[0] = 0;
                while(true)
                {
                    auto nextId = runDispatcher(effect, EffectOpcode::effectShellGetNextPlugin, 0, 0,
                        name, std::size(name));
                    if(nextId == 0 || name[0] == 0)
                    {
                        break;
                    }
                    uids.emplace_back(nextId);
                }
            }
            else
            {
                std::uint32_t versionFromDispatcher =
                    runDispatcher(effect, EffectOpcode::effectGetVendorVersion);
                std::uint32_t versionFromEffect;
                std::memcpy(&versionFromEffect, &effect->version,
                    sizeof(versionFromEffect));
                QString version =
                    (versionFromDispatcher == versionFromEffect || versionFromDispatcher == 0)?
                        YADAW::Audio::Util::getVersionString(versionFromEffect):
                        QString("%1 (%2)").arg(
                            YADAW::Audio::Util::getVersionString(versionFromEffect),
                            YADAW::Audio::Util::getVersionString(versionFromDispatcher)
                        );
                runDispatcher(effect, EffectOpcode::effectGetEffectName, 0, 0,
                    name, std::size(name));
                runDispatcher(effect, EffectOpcode::effectGetVendorName, 0, 0,
                    vendor, std::size(vendor));
                auto type = (effect->flags & EffectFlag::effectIsSynth)?
                            YADAW::DAO::PluginType::PluginTypeInstrument:
                            YADAW::DAO::PluginType::PluginTypeAudioEffect;
                YADAW::DAO::PluginInfo pluginInfo(
                    path,
                    uidAsVector,
                    name,
                    vendor,
                    version,
                    YADAW::DAO::PluginFormat::PluginFormatVestifal,
                    type
                );
                runDispatcher(effect, EffectOpcode::effectClose);
                return {PluginScanResult {pluginInfo, {}}};
            }
            std::vector<PluginScanResult> ret;
            ret.reserve(uids.size());
            for(auto uid: uids)
            {
                library = Library{path};
                name[0] = 0;
                vendor[0] = 0;
                auto plugin = YADAW::Audio::Util::createVestifalFromLibrary(library, uid);
                if(auto effect = plugin.effect(); effect)
                {
                    std::memcpy(uidAsVector.data(), &effect->uniqueId, sizeof(std::int32_t));
                    auto version = QString::number(effect->version, 16);
                    runDispatcher(effect, EffectOpcode::effectGetEffectName, 0, 0,
                        name, std::size(name));
                    runDispatcher(effect, EffectOpcode::effectGetVendorName, 0, 0,
                        vendor, std::size(vendor));
                    auto type = (effect->flags & EffectFlag::effectIsSynth)?
                               YADAW::DAO::PluginType::PluginTypeInstrument:
                               YADAW::DAO::PluginType::PluginTypeAudioEffect;
                    YADAW::DAO::PluginInfo pluginInfo(
                        path,
                        uidAsVector,
                        name,
                        vendor,
                        version,
                        YADAW::DAO::PluginFormat::PluginFormatVestifal,
                        type
                    );
                    ret.emplace_back(PluginScanResult {pluginInfo, {}});
                }
            }
            runDispatcher(effect, EffectOpcode::effectClose);
            plugin = {};
            library = {};
            return ret;
        }
    }
    return {};
}

void savePluginScanResult(const PluginScanResult& result)
{
    const auto& [info, categories] = result;
    int id = YADAW::DAO::insertPlugin(info);
    YADAW::DAO::insertPluginCategory(id, categories);
}

std::optional<
    std::pair<
        YADAW::Controller::LibraryPluginMap::iterator,
        PluginAndProcess
    >
> createPlugin(
    const QString& path, YADAW::DAO::PluginFormat format,
    const std::vector<char>& uid)
{
    using namespace YADAW::Audio::Plugin;
    auto& libraryPluginMap = YADAW::Controller::appLibraryPluginMap();
    auto it = libraryPluginMap.find<QString>(path);
    if(it == libraryPluginMap.end())
    {
        YADAW::Native::Library library(path);
        if(library.loaded())
        {
            it = libraryPluginMap.emplace(std::move(library),
                YADAW::Controller::LibraryPluginMap::value_type::second_type()
            ).first;
        }
        else
        {
            return std::nullopt;
        }
    }
    auto& library = it->first;
    switch(format)
    {
    case YADAW::DAO::PluginFormat::PluginFormatVST3:
    {
        if(uid.size() != sizeof(Steinberg::TUID) / sizeof(char))
        {
            break;
        }
        auto factoryEntry = library.getExport<VST3Plugin::FactoryEntry>("GetPluginFactory");
        if(!factoryEntry)
        {
            break;
        }
        auto init = library.getExport<YADAW::Native::VST3InitEntry>(YADAW::Native::initEntryName);
        auto exit = library.getExport<VST3Plugin::ExitEntry>(YADAW::Native::exitEntryName);
        auto vst3Plugin = std::make_unique<VST3Plugin>(
            init, factoryEntry, exit, library.handle()
        );
        if(vst3Plugin->status() == IAudioPlugin::Status::Loaded)
        {
            Steinberg::TUID tuid;
            std::memcpy(tuid, uid.data(), std::size(tuid));
            if(vst3Plugin->createPlugin(tuid))
            {
                auto& pluginToProcess = *vst3Plugin;
                it->second.emplace(std::move(vst3Plugin));
                return {
                    std::pair(
                        it,
                        PluginAndProcess(
                            &pluginToProcess,
                            YADAW::Audio::Engine::AudioDeviceProcess(pluginToProcess)
                        )
                    )
                };
            }
        }
        break;
    }
    case YADAW::DAO::PluginFormat::PluginFormatCLAP:
    {
        auto factoryEntry = library.getExport<const clap_plugin_entry*>("clap_entry");
        if(!factoryEntry)
        {
            break;
        }
        auto clapPlugin = std::make_unique<CLAPPlugin>(
            factoryEntry, path
        );
        if(clapPlugin->status() == IAudioPlugin::Status::Loaded
            && clapPlugin->createPlugin(uid.data()))
        {
            auto& pluginToProcess = *clapPlugin;
            it->second.emplace(std::move(clapPlugin));
            return {
                std::pair(
                    it,
                    PluginAndProcess(
                        &pluginToProcess,
                        YADAW::Audio::Engine::AudioDeviceProcess(pluginToProcess)
                    )
                )
            };
        }
        break;
    }
    case YADAW::DAO::PluginFormat::PluginFormatVestifal:
    {
        auto entry = YADAW::Audio::Util::vestifalEntryFromLibrary(library);
        if(!entry)
        {
            break;
        }
        std::int32_t id;
        std::memcpy(&id, uid.data(), sizeof(id));
        auto vestifalPlugin = std::make_unique<VestifalPlugin>(entry, id);
        if(vestifalPlugin->status() == IAudioPlugin::Status::Created)
        {
            auto& pluginToProcess = *vestifalPlugin;
            it->second.emplace(std::move(vestifalPlugin));
            return {
                std::pair(
                    it,
                    PluginAndProcess(
                        &pluginToProcess,
                        YADAW::Audio::Engine::AudioDeviceProcess(pluginToProcess)
                    )
                )
            };
        }
        break;
    }
    default:
        break;
    }
    return std::nullopt;
}

std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin> createPlugin(
    const YADAW::Native::Library& library,
    YADAW::DAO::PluginFormat format,
    const std::vector<char>& uid
)
{
    if(format == YADAW::DAO::PluginFormat::PluginFormatVST3)
    {
        if(uid.size() != sizeof(Steinberg::TUID) / sizeof(char))
        {
            return nullptr;
        }
        auto factoryEntry = library.getExport<VST3Plugin::FactoryEntry>("GetPluginFactory");
        if(!factoryEntry)
        {
            return nullptr;
        }
        auto init = library.getExport<YADAW::Native::VST3InitEntry>(YADAW::Native::initEntryName);
        auto exit = library.getExport<VST3Plugin::ExitEntry>(YADAW::Native::exitEntryName);
        auto vst3Plugin = std::make_unique<VST3Plugin>(
            init, factoryEntry, exit, library.handle()
        );
        if(vst3Plugin->status() == IAudioPlugin::Status::Loaded)
        {
            Steinberg::TUID tuid;
            std::memcpy(tuid, uid.data(), std::size(tuid));
            if(vst3Plugin->createPlugin(tuid))
            {
                return std::move(vst3Plugin);
            }
        }
    }
    else if(format == YADAW::DAO::PluginFormat::PluginFormatCLAP)
    {
        auto factoryEntry = library.getExport<const clap_plugin_entry*>("clap_entry");
        if(!factoryEntry)
        {
            return nullptr;
        }
        auto clapPlugin = std::make_unique<CLAPPlugin>(
            factoryEntry, library.path()
        );
        if(clapPlugin->status() == IAudioPlugin::Status::Loaded
            && clapPlugin->createPlugin(uid.data()))
        {
            return std::move(clapPlugin);
        }
    }
    else if(format == YADAW::DAO::PluginFormat::PluginFormatVestifal)
    {
        auto entry = YADAW::Audio::Util::vestifalEntryFromLibrary(library);
        if(!entry)
        {
            return nullptr;
        }
        std::int32_t id;
        std::memcpy(&id, uid.data(), sizeof(id));
        auto vestifalPlugin = std::make_unique<VestifalPlugin>(entry, id);
        if(vestifalPlugin->status() == IAudioPlugin::Status::Created)
        {
            return std::move(vestifalPlugin);
        }
    }
    return nullptr;
}

std::optional<PluginContext> createPluginWithContext(
    const QString& path, YADAW::DAO::PluginFormat format,
    const std::vector<char>& uid)
{
    auto& pool = appLibraryPluginPool();
    if(auto instance = pool.createPluginInstance(
        path,
        [format, &uid](const YADAW::Native::Library& library)
        {
            return createPlugin(library, format, uid);
        }
    ))
    {
        PluginContext ret {
            std::move(instance),
            YADAW::Audio::Engine::AudioDeviceProcess(),
            YADAW::Util::createUniquePtr(nullptr),
            nullptr,
            nullptr,
            YADAW::Util::createUniquePtr(nullptr)
        };
        if(format == YADAW::DAO::PluginFormat::PluginFormatVST3)
        {
            auto& plugin = static_cast<YADAW::Audio::Plugin::VST3Plugin&>(
                ret.pluginInstance.plugin()->get()
            );
            ret.process = YADAW::Audio::Engine::AudioDeviceProcess(plugin);
            ret.hostContext = YADAW::Util::createPMRUniquePtr(
                std::make_unique<YADAW::Audio::Host::VST3ComponentHandler>(plugin)
            );
        }
        else if(format == YADAW::DAO::PluginFormat::PluginFormatCLAP)
        {
            auto& plugin = static_cast<YADAW::Audio::Plugin::CLAPPlugin&>(
                ret.pluginInstance.plugin()->get()
            );
            ret.process = YADAW::Audio::Engine::AudioDeviceProcess();
            ret.hostContext = YADAW::Util::createPMRUniquePtr(
                std::make_unique<YADAW::Audio::Host::CLAPHost>(plugin)
            );
        }
        else if(format == YADAW::DAO::PluginFormat::PluginFormatVestifal)
        {
            auto& plugin = static_cast<YADAW::Audio::Plugin::VestifalPlugin&>(
                ret.pluginInstance.plugin()->get()
            );
            ret.process = YADAW::Audio::Engine::AudioDeviceProcess(plugin);
            // TODO
        }
        return ret;
    }
    return std::nullopt;
}
}
