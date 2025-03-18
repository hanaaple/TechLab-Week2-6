#include "TextureData.h"

FTextureStaticData* TextureData::GetTexture(ETextureType TextureType)
{
    if (!Textures.Contains(TextureType))
    {
        FTextureStaticData TextureStaticData;
        switch (TextureType)
        {
        case ETextureType::None:
            return nullptr;
            break;
        case ETextureType::TextureA:
            TextureStaticData.FileDirectory = FName("경로");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 512.0f;
            TextureStaticData.BitmapHeight = 512.0f;
            TextureStaticData.RowCount = 16;
            TextureStaticData.ColCount = 16;
            TextureStaticData.AtlasTotalCount = 256;
            Textures.Add(TextureType, TextureStaticData);
            break;
        case ETextureType::FontTexture:
            TextureStaticData.FileDirectory = FName("경로");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 512.0f;
            TextureStaticData.BitmapHeight = 512.0f;
            TextureStaticData.RowCount = 16;
            TextureStaticData.ColCount = 16;
            TextureStaticData.AtlasTotalCount = 256;
            Textures.Add(TextureType, TextureStaticData);
            break;
        case ETextureType::TextureC:
            TextureStaticData.FileDirectory = FName("경로");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 512.0f;
            TextureStaticData.BitmapHeight = 512.0f;
            TextureStaticData.RowCount = 16;
            TextureStaticData.ColCount = 16;
            TextureStaticData.AtlasTotalCount = 256;
            Textures.Add(TextureType, TextureStaticData);
            break;
        default:
            return nullptr;
            break;
        }
    }
        
    return &Textures[TextureType];
}