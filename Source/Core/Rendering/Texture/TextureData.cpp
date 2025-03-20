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
        case ETextureType::FontATexture:
            TextureStaticData.FileDirectory = FName("Resources/tempTexture.png");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 512.0f;
            TextureStaticData.BitmapHeight = 512.0f;
            TextureStaticData.RowCount = 1;
            TextureStaticData.ColCount = 1;
            TextureStaticData.AtlasTotalCount = 1;
            Textures.Add(TextureType, TextureStaticData);
            break;
        case ETextureType::AtlasFontTexture:
            TextureStaticData.FileDirectory = FName("Resources/tempTexture.png");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 512.0f;
            TextureStaticData.BitmapHeight = 512.0f;
            TextureStaticData.RowCount = 16;
            TextureStaticData.ColCount = 16;
            TextureStaticData.AtlasTotalCount = 256;
            Textures.Add(TextureType, TextureStaticData);
            break;
        case ETextureType::CatTexture:
            TextureStaticData.FileDirectory = FName("Resources/cat0.png");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 439.f;
            TextureStaticData.BitmapHeight = 439.0f;
            TextureStaticData.RowCount = 1;
            TextureStaticData.ColCount = 1;
            TextureStaticData.AtlasTotalCount = 1;
            Textures.Add(TextureType, TextureStaticData);
            break;
        case ETextureType::Lenna:
            TextureStaticData.FileDirectory = FName("Resources/Lenna.png");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 330.f;
            TextureStaticData.BitmapHeight = 330.0f;
            TextureStaticData.RowCount = 1;
            TextureStaticData.ColCount = 1;
            TextureStaticData.AtlasTotalCount = 1;
            Textures.Add(TextureType, TextureStaticData);
            break;
        case ETextureType::Cement:
            TextureStaticData.FileDirectory = FName("Resources/cement.png");
            TextureStaticData.TextureName = FName("Texture 명");
            TextureStaticData.BitmapWidth = 330.f;
            TextureStaticData.BitmapHeight = 225.0f;
            TextureStaticData.RowCount = 1;
            TextureStaticData.ColCount = 1;
            TextureStaticData.AtlasTotalCount = 1;
            Textures.Add(TextureType, TextureStaticData);
            break;
        default:
            return nullptr;
            break;
        }
    }
        
    return &Textures[TextureType];
}