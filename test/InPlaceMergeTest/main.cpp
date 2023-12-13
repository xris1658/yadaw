#include "common/DisableStreamBuffer.hpp"

#include "util/Algorithm.hpp"

#include <cstdio>
#include <new>

// This overload is added for testing `inPlaceMerge` with failed memory allocation
void* operator new[](std::size_t count, const std::nothrow_t&) noexcept
{
    return nullptr;
}

void checkMerge(int* array, int size, int middle)
{
    FOR_RANGE0(i, size)
    {
        std::printf("%d%s", array[i], i < size - 1? ", ": "\n");
    }
    auto inPlaceMergeResult = YADAW::Util::inPlaceMerge(array, array + middle, array + size);
    auto& [fof, lof, fom, lom] = inPlaceMergeResult;
    FOR_RANGE0(i, size)
    {
        std::printf("%c%s",
            fof - array == i? '[':
            ' ',
            i < size - 1? "  ": "\n");
    }
    FOR_RANGE0(i, size)
    {
        std::printf("%c%s",
            fom - array == i? '{':
            ' ',
            i < size - 1? "  ": "\n");
    }
    FOR_RANGE0(i, size)
    {
        std::printf("%c%s",
            lof - array == i? ']':
            ' ',
            i < size - 1? "  ": "\n");
    }
    FOR_RANGE0(i, size)
    {
        std::printf("%c%s",
            lom - array == i? '}':
            ' ',
            i < size - 1? "  ": "\n");
    }
    FOR_RANGE0(i, size)
    {
        std::printf("%d%s", array[i], i < size - 1? ", ": "\n\n");
    }
}

int main()
{
    disableStdOutBuffer();
    int array[10];
    FOR_RANGE0(i, 5)
    {
        array[i] = i * 2;
        array[i + 5] = i * 2 + 1;
    }
    checkMerge(array, 10, 5);
    FOR_RANGE0(i, 5)
    {
        array[i] = i + 5;
        array[i + 5] = i;
    }
    checkMerge(array, 10, 5);
    array[0] = 9;
    FOR_RANGE0(i, 9)
    {
        array[i + 1] = i;
    }
    checkMerge(array, 10, 1);
    array[9] = 0;
    FOR_RANGE0(i, 9)
    {
        array[i] = i + 1;
    }
    checkMerge(array, 10, 9);
    FOR_RANGE0(i, 10)
    {
        array[i] = i;
    }
    checkMerge(array, 10, 0);
    FOR_RANGE0(i, 10)
    {
        array[i] = i;
    }
    checkMerge(array, 10, 10);
}