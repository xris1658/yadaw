#include "LibraryPluginMap.hpp"

namespace YADAW::Controller
{
LibraryPluginMap& appLibraryPluginMap()
{
    static LibraryPluginMap ret;
    return ret;
}
}