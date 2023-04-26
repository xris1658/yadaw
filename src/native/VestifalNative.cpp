#include "VestifalNative.hpp"

namespace YADAW::Native
{
VestifalEntry createVEstifalFromLibrary(Library& library)
{
    auto ret = library.getExport<VestifalEntry>(VESTIFAL_ENTRY_NAME);
    if(!ret)
    {
        ret = library.getExport<VestifalEntry>("main");
    }
    return ret;
}
}