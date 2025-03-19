#pragma once
#include "Core/HAL/PlatformType.h"

enum class EShaderType : uint8
{
    None,
    DefaultShader,
    PickingShader,
    OutlineShader,
    TextShader,
    PrimitiveShader,
    CustomShader,  // 이후 추가될 사용자 정의 쉐이더
};
