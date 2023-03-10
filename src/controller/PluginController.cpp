#include "PluginController.hpp"

#include "audio/util/VST3Util.hpp"
#include "dao/PluginCategoryTable.hpp"
#include "dao/PluginTable.hpp"
#include "native/CLAPNative.hpp"
#include "native/VST3Native.hpp"
#include "util/Base.hpp"

#include <pluginterfaces/vst/ivstaudioprocessor.h>

#include <clap/plugin-features.h>

#include <QDir>
#include <QStringList>

#include <cstring>

namespace YADAW::Controller
{
using namespace YADAW::Audio::Device;
using namespace YADAW::Audio::Plugin;
using namespace YADAW::Native;

QLatin1StringView vst3Ext("vst3");
QLatin1StringView clapExt("clap");
QString vst3Pattern("*.vst3");
QString clapPattern("*.clap");

std::vector<QString> scanDirectory(const QDir& dir, bool recursive, bool includeSymLink)
{
    QStringList nameFilters({vst3Pattern, clapPattern});
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
                if(suffix == vst3Ext && suffix == clapExt)
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
    if(recursive)
    {
        const auto& dirList = dir.entryInfoList(QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::NoDotAndDotDot);
        for(auto& dirInfo: dirList)
        {
            if(dirInfo.isDir() && dirInfo.exists())
            {
                QDir subdir(dirInfo.absoluteFilePath());
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
    if(library.errorCode() != 0)
    {
        return {};
    }
    if(path.endsWith(vst3Ext, Qt::CaseSensitivity::CaseInsensitive))
    {
        auto plugin = createVST3FromLibrary(library);
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
                for(decltype(classCount) i = 0; i < classCount; ++i)
                {
                    bool isInstrument = false;
                    bool isAudioEffect = false;
                    std::memset(classInfoW.subCategories, 0, YADAW::Util::stackArraySize(classInfoW.subCategories));
                    if(factory3->getClassInfoUnicode(i, &classInfoW) == Steinberg::kResultOk
                    && std::strcmp(classInfoW.category, kVstAudioEffectClass) == 0)
                    {
                        char* subCategoryCollection[YADAW::Util::stackArraySize(classInfoW.subCategories)];
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
                        if(isInstrument)
                        {
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                                YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeInstrument);
                            ret.push_back({pluginInfo, subCategories});
                        }
                        if(isAudioEffect)
                        {
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                                YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeAudioEffect);
                            ret.push_back({pluginInfo, subCategories});
                        }
                        else if(!isInstrument)
                        {
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                                YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeUnknown);
                            ret.push_back({pluginInfo, subCategories});
                        }
                    }
                }
            }
            else if(factory2)
            {
                auto classCount = factory2->countClasses();
                Steinberg::PClassInfo2 classInfo2 {};
                for(decltype(classCount) i = 0; i < classCount; ++i)
                {
                    bool isInstrument = false;
                    bool isAudioEffect = false;
                    std::memset(classInfo2.subCategories, 0, YADAW::Util::stackArraySize(classInfo2.subCategories));
                    if(factory2->getClassInfo2(i, &classInfo2) == Steinberg::kResultOk
                    && std::strcmp(classInfo2.category, kVstAudioEffectClass) == 0)
                    {
                        char* subCategoryCollection[YADAW::Util::stackArraySize(classInfo2.subCategories)];
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
                        if(isInstrument)
                        {
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                                YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeInstrument);
                            ret.push_back({pluginInfo, subCategories});
                        }
                        if(isAudioEffect)
                        {
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                                YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeAudioEffect);
                            ret.push_back({pluginInfo, subCategories});
                        }
                        else if(!isInstrument)
                        {
                            YADAW::DAO::PluginInfo pluginInfo(path, uid, name, vendor, version,
                                YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeUnknown);
                            ret.push_back({pluginInfo, subCategories});
                        }
                    }
                }
            }
            else
            {
                auto classCount = factory->countClasses();
                Steinberg::PClassInfo classInfo {};
                for(decltype(classCount) i = 0; i < classCount; ++i)
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
                                        YADAW::DAO::PluginInfo pluginInfo(path, uid, name, {}, {},
                                            YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeAudioEffect);
                                        ret.push_back({pluginInfo, {}});
                                    }
                                    if(hasMainEventInput)
                                    {
                                        YADAW::DAO::PluginInfo pluginInfo(path, uid, name, {}, {},
                                            YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeInstrument);
                                        ret.push_back({pluginInfo, {}});
                                    }
                                    else if(!hasMainAudioInput)
                                    {
                                        YADAW::DAO::PluginInfo pluginInfo(path, uid, name, {}, {},
                                            YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeUnknown);
                                        ret.push_back({pluginInfo, {}});
                                    }
                                }
                            }
                            if(ai && ao)
                            {
                                YADAW::DAO::PluginInfo pluginInfo(path, uid, name, {}, {},
                                    YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeAudioEffect);
                                ret.push_back({pluginInfo, {}});
                            }
                            if(ei && eo)
                            {
                                YADAW::DAO::PluginInfo pluginInfo(path, uid, name, {}, {},
                                    YADAW::DAO::PluginFormatVST3, YADAW::DAO::PluginTypeMIDIEffect);
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
            factory->release();
            return ret;
        }
    }
    else if(path.endsWith(clapExt, Qt::CaseSensitivity::CaseInsensitive))
    {
        auto plugin = createCLAPFromLibrary(library);
        if(plugin.entry())
        {
            std::vector<PluginScanResult> ret;
            auto factory = plugin.factory();
            auto count = factory->get_plugin_count(factory);
            for(decltype(count) i = 0; i < count; ++i)
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
                    if(isInstrument)
                    {
                        YADAW::DAO::PluginInfo pluginInfo(
                            path, uid, name, vendor, version, YADAW::DAO::PluginFormatCLAP, YADAW::DAO::PluginTypeInstrument
                        );
                        ret.push_back({pluginInfo, featureCollection});
                    }
                    if(isAudioEffect)
                    {
                        YADAW::DAO::PluginInfo pluginInfo(
                            path, uid, name, vendor, version, YADAW::DAO::PluginFormatCLAP, YADAW::DAO::PluginTypeAudioEffect
                        );
                        ret.push_back({pluginInfo, featureCollection});
                    }
                    if(isMIDIEffect)
                    {
                        YADAW::DAO::PluginInfo pluginInfo(
                            path, uid, name, vendor, version, YADAW::DAO::PluginFormatCLAP, YADAW::DAO::PluginTypeMIDIEffect
                        );
                        ret.push_back({pluginInfo, featureCollection});
                    }
                }
            }
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
}
