#pragma once
#include "Texture.h"

class UTextureLoader {
public:
	UTextureLoader() = default;
	~UTextureLoader() = default;
	
	Texture* GetTexture(ETextureType TextureEnum, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);

private:
	static ID3D11ShaderResourceView* LoadTexture(FName FileName, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);

private:
	TMap<std::string, Texture> TextureMap;
};

//wstring StringToWString(const std::string& str)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//	return converter.from_bytes(str);
//}