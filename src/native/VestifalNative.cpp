#include "VestifalNative.hpp"

namespace YADAW::Native
{
YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(Library& library, std::int32_t uid)
{
    auto entry = library.getExport<VestifalEntry>(VESTIFAL_ENTRY_NAME);
    if(!entry)
    {
        entry = library.getExport<VestifalEntry>("main");
    }
    if(!entry)
    {
        return {};
    }
    return {entry, uid};
}
}