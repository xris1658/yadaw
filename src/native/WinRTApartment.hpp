#ifndef YADAW_SRC_NATIVE_WINRTAPARTMENT
#define YADAW_SRC_NATIVE_WINRTAPARTMENT

namespace YADAW::Native
{
class WinRTApartment
{
private:
    WinRTApartment();
public:
    static void init();
    ~WinRTApartment();
};
}

#endif //YADAW_SRC_NATIVE_WINRTAPARTMENT
