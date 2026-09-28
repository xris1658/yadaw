#include "util/Algorithm.hpp"

#include <array>
#include <cassert>
#include <ranges>


// 5 -> 3:
// old:   v
//      | 1 | 2 | 3 | 4 | 5 |
// new: | 3 | 4 | 5 |
// old:           v
//      | 1 | 2 | 3 | 4 | 5 |
// new: | 5 | 1 | 2 |
// old:               v
//      | 1 | 2 | 3 | 4 | 5 |
// new: | 1 | 2 | 3 |
// old:                   v
//      | 1 | 2 | 3 | 4 | 5 |
// new: | 2 | 3 | 4 |
// old:                       v
//      | 1 | 2 | 3 | 4 | 5 | x |
// new: | 3 | 4 | 5 |

// 5 -> 1:
// old:   v
//      | 1 | 2 | 3 | 4 | 5 |
// new: | 5 |
// old:           v
//      | 1 | 2 | 3 | 4 | 5 |
// new: | 2 |

void testRotateDropAndCopy()
{
    std::array<int, 5> in {1, 2, 3, 4, 5};
    std::array<int, 3> out;
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin(), in.end(), 2, out.begin()
    );
    assert(std::ranges::equal(out, std::array<int, 3>{{3, 4, 5}}));
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin() + 2, in.end(), 2, out.begin()
    );
    assert(std::ranges::equal(out, std::array<int, 3>{{5, 1, 2}}));
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin() + 3, in.end(), 2, out.begin()
    );
    assert(std::ranges::equal(out, std::array<int, 3>{{1, 2, 3}}));
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin() + 4, in.end(), 2, out.begin()
    );
    assert(std::ranges::equal(out, std::array<int, 3>{{2, 3, 4}}));
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.end(), in.end(), 2, out.begin()
    );
    assert(std::ranges::equal(out, std::array<int, 3>{{3, 4, 5}}));
    int out2;
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin(), in.end(), 4, &out2
    );
    assert(out2 == 5);
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin() + 2, in.end(), 4, &out2
    );
    assert(out2 == 2);
    out2 = -1;
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin(), in.end(), 5, &out2
    );
    assert(out2 == -1);
    YADAW::Util::rotateDropAndCopy(
        in.begin(), in.begin() + 2, in.end(), 5, &out2
    );
    assert(out2 == -1);
}

int main()
{
    testRotateDropAndCopy();
}