#pragma once
#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>

#include <string>
#include <Core/Container/Map.h>

#include "Core/AbstractClass/Singleton.h"
#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/x64/Debug/DirectXTKd.lib")  // 디버그용 라이브러리
#else
#pragma comment(lib, "DirectXTK/x64/Release/DirectXTK.lib") // 릴리즈용 라이브러리
#endif
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

	// png 기반 텍스처 로드
	bool LoadTexture(string fileName);
	// 아틀라스 별 uv 좌표 값 저장
	void LoadCharInfo(float bitmapWidth, float bitmapHeight, float rowSize, float colSize, int rowNum, int colNum);
	ID3D11ShaderResourceView* GetTextureSRV();
	void DrawText(const std::string& text);
	TMap<char, CharacterInfo> charInfoMap;

	// FIXME : 텍스처 캐싱으로 바꾸기
	ID3D11ShaderResourceView* m_texture;
};

//wstring StringToWString(const std::string& str)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//	return converter.from_bytes(str);
//}