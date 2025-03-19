#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>

#include "TextureData.h"


struct TextureInfo {
    float u;		// u 좌표 ( 텍스처 내 위치 )
    float v;		// v 좌표 ( 텍스처 내 위치 )
    float width;	// 문자의 너비 ( 픽셀 또는 uv 스케일)
    float height;	// 문자의 높이 ( 픽셀 또는 uv 스케일)
};

struct Texture
{
public:

    void Initialize(const FTextureStaticData& TextureData);
    
    ID3D11ShaderResourceView* GetTextureSRV() { return TextureSRV; }
    void SetTexture(ID3D11ShaderResourceView* Texture) { TextureSRV = Texture; }
    TextureInfo GetCharInfoMap(char character);

    FTextureStaticData TextureData;

private:
    ID3D11ShaderResourceView* TextureSRV;
    TMap<char, TextureInfo> AtlasInfoMap;
};
