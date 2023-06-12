#ifndef YADAW_SRC_UTIL_YAMLCPPUTIL
#define YADAW_SRC_UTIL_YAMLCPPUTIL

#include <yaml-cpp/emitter.h>

namespace YADAW::Util
{
template<YAML::EMITTER_MANIP Begin, YAML::EMITTER_MANIP End>
class YAMLBeginEndHelper
{
public:
    YAMLBeginEndHelper(YAML::Emitter& emitter): emitter_(emitter)
    {
        emitter_ << Begin;
    }
    ~YAMLBeginEndHelper()
    {
        emitter_ << End;
    }
private:
    YAML::Emitter& emitter_;
};

using YAMLSeq = YAMLBeginEndHelper<YAML::BeginSeq, YAML::EndSeq>;
using YAMLMap = YAMLBeginEndHelper<YAML::BeginMap, YAML::EndMap>;
using YAMLDoc = YAMLBeginEndHelper<YAML::BeginDoc, YAML::EndDoc>;
}

#endif // YADAW_SRC_UTIL_YAMLCPPUTIL
