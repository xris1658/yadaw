#include "VestifalNative.hpp"

namespace YADAW::Native
{
QLatin1StringView vestifalExt("dll");
QString vestifalPattern("*.dll");

VestifalEntry vestifalEntryFromLibrary(Library& library)
{
    auto entry = library.getExport<VestifalEntry>(VESTIFAL_ENTRY_NAME);
    if(!entry)
    {
        entry = library.getExport<VestifalEntry>("main");
    }
    return entry;
}

YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(Library& library, std::int32_t uid)
{
    auto entry = vestifalEntryFromLibrary(library);
    if(!entry)
    {
        return {};
    }
    return {entry, uid};
}
}