#ifndef YADAW_SRC_CONTROLLER_GENERALSETTINGSCONTROLLER
#define YADAW_SRC_CONTROLLER_GENERALSETTINGSCONTROLLER

namespace YADAW::Controller
{
class GeneralSettingsController
{
public:
    static bool systemFontRendering();
    static void setSystemFontRendering(bool enabled);
};
}

#endif //YADAW_SRC_CONTROLLER_GENERALSETTINGSCONTROLLER
