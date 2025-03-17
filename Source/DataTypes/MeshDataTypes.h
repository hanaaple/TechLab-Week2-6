#pragma once
#include "Core/HAL/PlatformType.h"

enum class EPrimitiveMeshType : uint8
{
    EPT_None,
    EPT_Triangle,
    EPT_Cube,
    EPT_Sphere,
    EPT_Line,
    EPT_BoundingBox,
    EPT_GridLine,
    EPT_Cylinder,
    EPT_Cone,
    EPT_Torus,
    EPT_Max,
};
