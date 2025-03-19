#pragma once
#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>

#include "Primitive/MeshResourceCache.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "DataTypes/ShdaerType.h"
#include "Texture/TextureData.h"

class UPrimitiveComponent;

using bUseIndexBufferFlag = bool;

struct FBatchRenderContext
{
    FBatchRenderContext() = default;
    bool bIsDirty = true;

    // Vertex Data가 변했는지
    // Index Data가 변했는지 확인.
    
    EShaderType ShaderType;
    ETextureType TextureType;
    D3D11_PRIMITIVE_TOPOLOGY Topology;
    bUseIndexBufferFlag bUseIndexBuffer;

    TMap<EPrimitiveMeshType, TArray<UPrimitiveComponent*>> RenderComponentMap;
};