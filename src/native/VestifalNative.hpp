#ifndef YADAW_SRC_NATIVE_VESTIFALNATIVE
#define YADAW_SRC_NATIVE_VESTIFALNATIVE

#include "audio/plugin/vestifal/Vestifal.h"
#include "native/Library.hpp"

namespace YADAW::Native
{
VestifalEntry createVEstifalFromLibrary(Library& library);
}

#endif //YADAW_SRC_NATIVE_VESTIFALNATIVE
