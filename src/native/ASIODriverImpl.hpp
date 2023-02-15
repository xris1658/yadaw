#ifndef YADAW_SRC_NATIVE_ASIODRIVERIMPL
#define YADAW_SRC_NATIVE_ASIODRIVERIMPL

#include <combaseapi.h>

#include <common/iasiodrv.h>

namespace YADAW::Native
{
class ASIODriverImpl
{
private:
    IASIO* driver_ = nullptr;
};
}

#endif //YADAW_SRC_NATIVE_ASIODRIVERIMPL
