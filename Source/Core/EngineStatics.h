#pragma once
#include "HAL/PlatformType.h"


class UEngineStatics
{
public:
    static uint32 GenUUID()
    {
        return NextUUID++;
    }

    static uint32 NextUUID;
};
