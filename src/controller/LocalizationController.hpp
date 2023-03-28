#ifndef YADAW_SRC_CONTROLLER_LOCALIZATIONCONTROLLER
#define YADAW_SRC_CONTROLLER_LOCALIZATIONCONTROLLER

#include "model/LocalizationListModel.hpp"
#include "l10n/L10N.hpp"

namespace YADAW::Controller
{
extern int currentTranslationIndex;

YADAW::Model::LocalizationListModel& appLocalizationListModel();
}

#endif //YADAW_SRC_CONTROLLER_LOCALIZATIONCONTROLLER
