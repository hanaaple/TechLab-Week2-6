﻿#include "TextureLoader.h"

#include "Debug/DebugConsole.h"
#include "DirectXTK/DirectXHelpers.h"
#include "DirectXTK/WICTextureLoader.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/x64/Debug/DirectXTKd.lib")  // 디버그용 라이브러리
#else
#pragma comment(lib, "DirectXTK/x64/Release/DirectXTK.lib") // 릴리즈용 라이브러리
#endif

Texture* UTextureLoader::GetTexture(ETextureType TextureEnum, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext)
{
	FTextureStaticData* TextureData = TextureData::Get().GetTexture(TextureEnum);
	if (TextureData == nullptr)
	{
		return nullptr;
	}

	std::string TextureName = TextureData->TextureName.ToStdString();
	
	if (!TextureMap.Contains(TextureEnum))
	{
		if (ID3D11ShaderResourceView* TextureSRV = LoadTexture(TextureData->FileDirectory, Device, DeviceContext))
		{
			TextureMap.Add(TextureEnum, Texture());
			TextureMap[TextureEnum].SetTexture(TextureSRV);
			TextureMap[TextureEnum].Initialize(*TextureData);
		}
		else
		{
			return nullptr;
		}
	}
	
	return &TextureMap[TextureEnum];
}

/* png 기반 텍스처 로드 */
ID3D11ShaderResourceView* UTextureLoader::LoadTexture(FName FileName, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext)
{
	auto a = FileName.ToStdString();
	const char* Data = FileName.ToCharString();
	ID3D11ShaderResourceView* TextureSRV;
	// 폰트 텍스처 로드 (DirectXTK 사용)
#pragma region png로드
	//int size_needed = MultiByteToWideChar(CP_UTF8, 0, Data, -1, nullptr, 0);
	//wchar_t* wstr = new wchar_t[size_needed]; // 동적 할당
	//MultiByteToWideChar(CP_UTF8, 0, Data, -1, wstr, size_needed);

	const char* directory = a.c_str();
	std::wstring aa = std::wstring(directory, directory + strlen(directory));
	
	HRESULT result = DirectX::CreateWICTextureFromFile(Device, DeviceContext, aa.c_str(), nullptr, &TextureSRV);
	if (FAILED(result))
	{
		UE_LOG("Error - Texture Load .png %s", FileName.ToCharString());
	}
#pragma endregion
	//FIXME : 런타임이 파일이름 넘기도록 변경.
	// DDS 형식의 텍스처 파일 로드 기능.

	//std::wstring wideFileName = L"C:\\Users\\Jungle\\Documents\\GitHub\\TechLab-Week2-6\\Resources\\DejaVuSansMono.dds";
	/*HRESULT result = DirectX::CreateDDSTextureFromFile(
		device,
		context,
		wideFileName.c_str(),
		nullptr,
		&m_texture
	);*/

	return TextureSRV;
}