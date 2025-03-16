#pragma once
#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>

#include "DirectXTK/DDSTextureLoader.h"
//#include "DirectXTex/SpriteFont.h"				// DirectXTK에서 제공하는 폰트 렌더링 클래스. .spritefont 형식의 폰트 데이터를 로드하고 글자를 화면에 그릴 수 있도록 함.
#include "DirectXTK/SpriteBatch.h"			// DirectXTK의 스프라이트 렌더링 도우미 클래스. 2D 텍스처(스프라이트)를 화면에 쉽게 렌더링.
//#include "DirectXTex/WICTextureLoader.h"
#include <Core/Container/Map.h>
#include <Core/Container/String.h>
#include <DirectXTK/SpriteBatch.h>
#include "Core/AbstractClass/Singleton.h"
#include "Core/Engine.h"
#include <string>
#include <locale>
//#include <codecvt>

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

	void DrawText(const std::string& text);

	TMap<char, CharacterInfo> charInfoMap;

	// FIXME : 텍스처 캐싱으로 바꾸기
	ID3D11ShaderResourceView* m_texture;
	//std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	void CreateQuad(float x, float y, float size);
};

//wstring StringToWString(const std::string& str)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//	return converter.from_bytes(str);
//}