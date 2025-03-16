#define NUM_LETTER 256
#include "TextureLoader.h"
#include <DirectXTK/WICTextureLoader.h>
#include "Core/Engine.h"

UTextureLoader::UTextureLoader()
{

}

UTextureLoader::UTextureLoader(const UTextureLoader&)
{

}

UTextureLoader::~UTextureLoader()
{
}

bool UTextureLoader::LoadTexture(string fileName)
{
	if (m_texture != nullptr)
		return true;
	// FIXME : 추후 텍스처 여러개 쓸 때 수정 필요.
	//LoadCharInfo(512.0f, 512.0f, 32.0f, 32.0f, 16, 16);

	// 폰트 텍스처 로드 (DirectXTK 사용)
#pragma region png로드
	/* png 로드 */
	//std::wstring wideFileName = StringToWString(fileName);
	//spriteFont = std::make_unique<DirectX::SpriteFont>(device, wideFileName);
	//std::wstring wideFileName = L"Resources/tempTexture.png";
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, nullptr, 0);
	wchar_t* wstr = new wchar_t[size_needed]; // 동적 할당
	MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), -1, wstr, size_needed);
	HRESULT result = DirectX::CreateWICTextureFromFile(UEngine::Get().GetRenderer()->GetDevice(), UEngine::Get().GetRenderer()->GetDeviceContext(), wstr, nullptr, &m_texture);
#pragma endregion
	//FIXME : 런타임이 파일이름 넘기도록 변경.
	//std::wstring wideFileName = L"C:\\Users\\Jungle\\Documents\\GitHub\\TechLab-Week2-6\\Resources\\DejaVuSansMono.dds";
	/*HRESULT result = DirectX::CreateDDSTextureFromFile(
		device,
		context,
		wideFileName.c_str(),
		nullptr,
		&m_texture
	);*/


	return true;

	// DDS 형식의 텍스처 파일 로드 기능.
	// 텍스처를 GPU에 올리고, ID3D11ShaderResourceView 생성하여 사용 가능하도록.
	//DirectX::CreateDDSTextureFromFile(device, wideFileName, nullptr, &m_texture);
}
//girdSize : 한 행과 한 열의 문자 개수
void UTextureLoader::LoadCharInfo(float bitmapWidth, float bitmapHeight, float rowSize, float colSize, int rowNum, int colNum)
{
	float cellWidthUV = colSize / bitmapWidth;
	float cellHeightUV = rowSize / bitmapHeight;

	for (int idx = 0; idx < NUM_LETTER; idx++) {
		int row = idx / colNum;		// 현재 문자가 속한 행 인덱스
		int col = idx % colNum;		// 현재 문자가 속한 열 인덱스

		/*/ col * colSize : 한 칸 가로 길이 (픽셀 단위)
		* row * rowSize : 한 칸 세로 길이 (픽셀 단위)
		* / bitmapWidth, bitmapHeight : 0 ~ 1로 정규화.
		directX는 좌표계까 좌측 상단부터(0, 0)이라서 방향 반전.*/
		float u = (col * colSize) / bitmapWidth;
		//float v = ((row * rowSize) / bitmapHeight);
		float v = 1.0f - ((row * rowSize) / bitmapHeight) - cellHeightUV;

		charInfoMap.Add(idx, { u, v, cellWidthUV, cellHeightUV });
	}
}

ID3D11ShaderResourceView* UTextureLoader::GetTextureSRV()
{
	return m_texture;
}


void UTextureLoader::DrawText(const std::string& text) {
	LoadCharInfo(512.0f, 512.0f, 32.0f, 32.0f, 16, 16);
	//for (char ch : text) {
	//	if (charInfoMap.Contains(ch)) {
	//		CharacterInfo charInfo = charInfoMap[ch];
	//		// 쉐이더에 update constant 해주기
	//	}
	//}

	UEngine::Get().GetRenderer()->UpdateConstantUV('!');
}