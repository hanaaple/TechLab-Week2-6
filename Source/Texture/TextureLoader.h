#pragma once
#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>

#include <string>
#include <Core/Container/Map.h>

#include "Core/AbstractClass/Singleton.h"

//FIXME : 디버그 모드만 추가 Release 모드도 추가하기
#pragma comment(lib, "DirectXTK/x64/Debug/DirectXTKd.lib")

using namespace std;

struct CharacterInfo {
	float u;		// u 좌표 ( 텍스처 내 위치 )
	float v;		// v 좌표 ( 텍스처 내 위치 )
	float width;	// 문자의 너비 ( 픽셀 또는 uv 스케일)
	float height;	// 문자의 높이 ( 픽셀 또는 uv 스케일)
};

class UTextureLoader : public TSingleton<UTextureLoader> {
public:
	UTextureLoader();
	UTextureLoader(const UTextureLoader&);
	~UTextureLoader();

	bool LoadTexture(string fileName);
	void LoadCharInfo(float bitmapWidth, float bitmapHeight, float rowSize, float colSize, int rowNum, int colNum);
	ID3D11ShaderResourceView* GetTextureSRV();
	void DrawText(const std::string& text);
	TMap<char, CharacterInfo> charInfoMap;

	// FIXME : 텍스처 캐싱으로 바꾸기
	ID3D11ShaderResourceView* m_texture;
	//std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	//void CreateQuad(float x, float y, float size);
};

//wstring StringToWString(const std::string& str)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//	return converter.from_bytes(str);
//}