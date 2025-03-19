#pragma once
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

struct FConstantBufferContext
{
public:
    UPrimitiveComponent* PrimitiveComponent = nullptr;

    FBatchRenderContext* BatchContext = nullptr;
};
