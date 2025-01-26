#include "MMCSSThreadTaskAssociator.hpp"

#include "util/IntegerRange.hpp"
#include "util/VectorTypes.hpp"

#include <windows.h>
#include <avrt.h>
#include <winreg.h>

#include <algorithm>
#include <mutex>

namespace YADAW::Native
{
Vec<Vec<char>, DWORD> tasks;

std::once_flag getTasksFlag;

void getTasks()
{
    HKEY hKey = nullptr;
    auto openKeyResult = RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile\\Tasks\\Pro Audio",
        0,
        KEY_READ, &hKey
    );
    if(openKeyResult == ERROR_SUCCESS)
    {
        DWORD numSubKey;
        DWORD maxSubKeyLength;
        auto queryInfoKeyResult = RegQueryInfoKeyA(hKey, NULL, NULL, NULL,
            &numSubKey, &maxSubKeyLength, NULL, NULL, NULL, NULL, NULL, NULL);
        if(queryInfoKeyResult == ERROR_SUCCESS)
        {
            ++maxSubKeyLength;
            tasks.reserve(numSubKey);
            Vec<char> keyName(maxSubKeyLength, 0);
            FOR_RANGE0(i, numSubKey)
            {
                DWORD keyLength;
                HKEY subKey = nullptr;
                auto enumKeyResult = RegEnumKeyExA(
                    hKey, i, keyName.data(), &keyLength, NULL, NULL, NULL, NULL
                );
                if(enumKeyResult == ERROR_SUCCESS)
                {
                    auto openSubKeyResult = RegOpenKeyExA(
                        hKey, keyName.data(), 0, KEY_READ, &subKey
                    );
                    if(openSubKeyResult == ERROR_SUCCESS)
                    {
                        DWORD priority;
                        DWORD dataSize = sizeof(priority);
                        auto queryValueResult = RegGetValueA(
                            subKey, NULL, "Priority", RRF_RT_REG_DWORD, NULL, &priority, &dataSize
                        );
                        if(queryValueResult == ERROR_SUCCESS)
                        {
                            tasks.emplace_back(
                                std::move(keyName), std::move(priority)
                            );
                        }
                        RegCloseKey(subKey);
                    }
                }
            }
        }
        RegCloseKey(hKey);
    }
    using ValueRef = decltype(tasks)::const_reference;
    std::sort(
        tasks.rbegin(), tasks.rend(),
        [](ValueRef lhs, ValueRef rhs)
        {
            return std::get<1>(lhs) < std::get<1>(rhs);
        }
    );
}

MMCSSThreadTaskAssociator::MMCSSThreadTaskAssociator()
{
    std::call_once(getTasksFlag, &getTasks);
    if(!tasks.empty())
    {
        handle_ = AvSetMmThreadCharacteristicsA(
            std::get<0>(tasks.front()).data(),
            &taskIndex_
        );
    }
}

MMCSSThreadTaskAssociator::~MMCSSThreadTaskAssociator()
{
    if(handle_ != 0)
    {
        AvRevertMmThreadCharacteristics(handle_);
    }
}
}
