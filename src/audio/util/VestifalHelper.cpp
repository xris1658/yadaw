#include "VestifalHelper.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
VestifalEntry vestifalEntryFromLibrary(const YADAW::Native::Library& library)
{
    auto entry = library.getExport<VestifalEntry>(VESTIFAL_ENTRY_NAME);
    if(!entry)
    {
        entry = library.getExport<VestifalEntry>("main");
    }
    return entry;
}

YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(const YADAW::Native::Library& library, std::int32_t uid)
{
    auto entry = vestifalEntryFromLibrary(library);
    if(!entry)
    {
        return {};
    }
    return {entry, uid};
}

QString getVersionString(std::uint32_t version)
{
    std::uint8_t versions[4];
    FOR_RANGE0(i, 4)
    {
        versions[i] = ((version >> (24 - 8 * i)) & 0x000000FF);
    }
    int offset = 3;
    FOR_RANGE0(i, 3)
    {
        if(versions[i] != 0)
        {
            offset = i;
            break;
        }
    }
    char versionString[16];
    char* versionStringOffset = versionString;
    FOR_RANGE(i, offset, 4)
    {
        versionStringOffset += std::sprintf(versionStringOffset, "%u.", versions[i]);
    }
    *(versionStringOffset - 1) = 0;
    return QString::fromLatin1(versionString);
}
}
