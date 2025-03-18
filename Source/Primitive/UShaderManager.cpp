#include "UShaderManager.h"

#include <filesystem>

UShader* UShaderManager::LoadShader(ID3D11Device* Device, const wchar_t* FileName)
{
    //파일 경로에서 파일명만 추출
    std::filesystem::path FilePath(FileName);
    FString ShaderNameStr = FilePath.stem().string(); // 확장자 제거한 파일명
    FName ShaderName(ShaderNameStr);

    return LoadShader(Device, ShaderName, FileName, "mainVS", "mainPS");
}
UShader* UShaderManager::LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry)
{
    if (ShaderMap.Find(Name.GetDisplayIndex()) != nullptr)
        return ShaderMap[Name.GetDisplayIndex()];

    UShader* NewShader = new UShader();
    NewShader->SetShaderName(Name);
    if (!NewShader->LoadShader(Device, FileName, *VertexEntry, *PixelEntry)) // FString을 TCHAR*로 변환
    {
        delete NewShader;
        return nullptr;
    }
    ShaderMap[Name.GetDisplayIndex()] = NewShader;
    return NewShader;
}

UShader* UShaderManager::GetShader(const FName& Name)
{
    auto it = ShaderMap.Find(Name.GetDisplayIndex());
    return *it;
}

void UShaderManager::ReleaseAll()
{

}
