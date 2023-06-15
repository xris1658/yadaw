#include "VestifalHelper.hpp"

namespace YADAW::Audio::Util
{
VestifalEntry vestifalEntryFromLibrary(YADAW::Native::Library& library)
{
    auto entry = library.getExport<VestifalEntry>(VESTIFAL_ENTRY_NAME);
    if(!entry)
    {
        entry = library.getExport<VestifalEntry>("main");
    }
    return entry;
}

YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(YADAW::Native::Library& library, std::int32_t uid)
{
    auto entry = vestifalEntryFromLibrary(library);
    if(!entry)
    {
        return {};
    }
    return {entry, uid};
}
}
