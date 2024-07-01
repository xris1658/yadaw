#include "LocalizationController.hpp"

#include "dao/Config.hpp"

#include <QDir>

#include <mutex>

namespace YADAW::Controller
{
namespace Impl
{
std::once_flag initializeLocalizationModelFlag;

void initializeLocalizationListModel(YADAW::Model::LocalizationListModel& model)
{
    model.clear();
    model.append({"English", "xris1658", "en_US", {}, {}, {}});
    QDir dir(YADAW::L10N::l10nDirectoryPath());
    if(dir.exists())
    {
        const auto& entryInfoList = dir.entryInfoList(
            QDir::Filter::AllDirs | QDir::Filter::Hidden | QDir::Filter::NoDotAndDotDot);
        for(const auto& entryInfo: entryInfoList)
        {
            QDir subdir(entryInfo.absoluteFilePath());
            const auto& fileEntryInfoList = subdir.entryInfoList({"*.yml"}, QDir::Filter::Files | QDir::Filter::Hidden);
            for(const auto& yamlFileInfo: fileEntryInfoList)
            {
                if(yamlFileInfo.exists())
                {
                    const auto& node = YADAW::DAO::loadConfig(yamlFileInfo.absoluteFilePath());
                    try
                    {
                        auto name = QString::fromStdString(node["name"].as<std::string, std::string>({}));
                        auto author = QString::fromStdString(node["author"].as<std::string, std::string>({}));
                        auto languageCode = QString::fromStdString(node["language-code"].as<std::string, std::string>({}));
                        auto translationFileListNode = node["translation-file-list"];
                        if(!translationFileListNode.IsSequence())
                        {
                            return;
                        }
                        std::vector<QString> translationFileList;
                        translationFileList.reserve(translationFileListNode.size());
                        for(const auto& i: translationFileListNode)
                        {
                            auto translationFileName = QString::fromStdString(i.as<std::string, std::string>({}));
                            translationFileList.emplace_back(subdir.absoluteFilePath(translationFileName));
                        }
                        std::vector<QString> fontFamilyList;
                        const auto& fontFamilyListNode = node["font-family"];
                        fontFamilyList.reserve(fontFamilyListNode.size());
                        for(const auto& i: fontFamilyListNode)
                        {
                            fontFamilyList.emplace_back(QString::fromStdString(i.as<std::string, std::string>({})));
                        }
                        std::vector<QString> fontList;
                        const auto& fontListNode = node["font-list"];
                        fontList.reserve(fontListNode.size());
                        for(const auto& i: fontListNode)
                        {
                            auto fileName = QString::fromStdString(i.as<std::string, std::string>({}));
                            fontList.emplace_back(subdir.absoluteFilePath(fileName));
                        }
                        model.append(
                            YADAW::Model::LocalizationListModel::Item {
                                name, author, languageCode, translationFileList, fontFamilyList, fontList
                            }
                        );
                    }
                    catch(...)
                    {}
                }
            }
        }
    }
}
}

int currentTranslationIndex = -1;

YADAW::Model::LocalizationListModel& appLocalizationListModel()
{
    static YADAW::Model::LocalizationListModel ret;
    std::call_once(Impl::initializeLocalizationModelFlag,
        &Impl::initializeLocalizationListModel, std::ref(ret));
    return ret;
}
}
