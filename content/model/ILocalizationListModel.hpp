#ifndef YADAW_CONTENT_MODEL_ILOCALIZATIONLISTMODEL
#define YADAW_CONTENT_MODEL_ILOCALIZATIONLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class ILocalizationListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        Author,
        LanguageCode,
        TranslationFileList,
        FontFamilyList,
        FontList,
        RoleCount
    };
public:
    ILocalizationListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~ILocalizationListModel() override {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name,                "lm_name"),
            std::make_pair(Role::Author,              "lm_author"),
            std::make_pair(Role::LanguageCode,        "lm_language_code"),
            std::make_pair(Role::TranslationFileList, "lm_translation_file_list"),
            std::make_pair(Role::FontFamilyList,      "lm_font_family_list"),
            std::make_pair(Role::FontList,            "lm_font_list")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_ILOCALIZATIONLISTMODEL
