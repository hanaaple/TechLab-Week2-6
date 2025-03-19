#pragma once

#include "Core/Name/FName.h"

enum class ETextureType{
    None,
    TextureA,
    FontTexture,
    TextureC
};

struct FTextureStaticData
{
    FName FileDirectory;
    FName TextureName;
    float BitmapWidth;
    float BitmapHeight;
    int RowCount;
    int ColCount;
    int AtlasTotalCount;
};

// Texture 마다
// 아틀라싱 여부에 따른 아틀라싱 사이즈 및 데이터
class TextureData : public TSingleton<TextureData>
{
public:

    FTextureStaticData* GetTexture(ETextureType TextureType);
    
private:
    TMap<ETextureType, FTextureStaticData> Textures;
};