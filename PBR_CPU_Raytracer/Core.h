#pragma once

#include <intrin.h>
#include <string>

inline void AssertIfTrue(bool arg)
{
    if(!arg)
        return;
    __debugbreak();
}

inline void MsgAssertIfTrue(bool arg, const std::string& err)
{
    if(!arg)
        return;
    std::cerr << err << std::endl;
    __debugbreak();
}