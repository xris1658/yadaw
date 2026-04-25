#ifndef YADAW_SRC_UTIL_BOOLFLAG
#define YADAW_SRC_UTIL_BOOLFLAG

namespace YADAW::Util
{
class BoolFlag
{
public:
    BoolFlag(bool& flag): flag_(flag) { flag_ = true; }
    ~BoolFlag() { flag_ = false; }
private:
    bool& flag_;
};
}

#endif // YADAW_SRC_UTIL_BOOLFLAG
