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

    // 아래 데이터들이 변한 경우 Dirty
    bool bIsDirty = true;

    
    //TODOOOOOOOO if Depth > 0 Check

    // Depth 무시 여부에 따라 배치 추가 군집화?
    
    
    EShaderType ShaderType;
    ETextureType TextureType;
    D3D11_PRIMITIVE_TOPOLOGY Topology;
    bUseIndexBufferFlag bUseIndexBuffer;

    TMap<EPrimitiveMeshType, TArray<UPrimitiveComponent*>> RenderComponentMap;
};