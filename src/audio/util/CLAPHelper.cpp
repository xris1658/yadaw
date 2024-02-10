#include "CLAPHelper.hpp"

namespace YADAW::Audio::Util
{
YADAW::Audio::Plugin::CLAPPlugin createCLAPFromLibrary(const YADAW::Native::Library& library)
{
    auto entry = library.getExport<const clap_plugin_entry*>("clap_entry");
    if(entry)
    {
        return YADAW::Audio::Plugin::CLAPPlugin(entry, library.path());
    }
    return {};
}
}