#ifndef YADAW_SRC_CONTROLLER_GENERALSETTINGSCONTROLLER
#define YADAW_SRC_CONTROLLER_GENERALSETTINGSCONTROLLER

#include <QString>

namespace YADAW::Controller
{
class GeneralSettingsController
{
public:
    static bool systemFontRendering();
    static void setSystemFontRendering(bool enabled);
    static void setTranslation(const QString& name);
};
}

#endif //YADAW_SRC_CONTROLLER_GENERALSETTINGSCONTROLLER
