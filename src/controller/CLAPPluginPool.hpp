#ifndef YADAW_SRC_CONTROLLER_CLAPPLUGINPOOL
#define YADAW_SRC_CONTROLLER_CLAPPLUGINPOOL

#include "audio/host/CLAPHost.hpp"
#include "audio/host/CLAPEventList.hpp"

#include <map>
#include <vector>

namespace YADAW::Audio::Plugin
{
class CLAPPlugin;
}

namespace YADAW::Controller
{
struct CLAPPluginContext
{
    YADAW::Audio::Host::CLAPEventList* eventList;
};

using CLAPPluginPool = std::map<
    YADAW::Audio::Plugin::CLAPPlugin*,
    CLAPPluginContext
>;

using CLAPPluginToSetProcess = std::pair<
    YADAW::Audio::Plugin::CLAPPlugin*,
    bool
>;

using CLAPPluginPoolVector = std::vector<
    CLAPPluginPool::value_type*
>;

using CLAPPluginToSetProcessVector = std::vector<
    CLAPPluginToSetProcess
>;

CLAPPluginPool& appCLAPPluginPool();

CLAPPluginPoolVector createPoolVector(CLAPPluginPool& pool);

void fillCLAPInputParameterChanges(CLAPPluginPoolVector& pool,
    std::int64_t callbackTimestampInNanosecond);

}

#endif // YADAW_SRC_CONTROLLER_CLAPPLUGINPOOL
