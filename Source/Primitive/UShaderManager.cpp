#include "UShaderManager.h"

UShader* UShaderManager::LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName, const char* VertexEntry, const char* PixelEntry)
{
    if (ShaderMap.find(Name) != ShaderMap.end())
        return ShaderMap[Name];
    
    UShader* NewShader = new UShader();
    if (!NewShader->LoadShader(Device, FileName, VertexEntry, PixelEntry))
    {
        delete NewShader;
        return nullptr;
    }
    ShaderMap[Name] = NewShader;
    return NewShader;
}

UShader* UShaderManager::GetShader(const FName& Name)
{
    auto it = ShaderMap.find(Name);
    return it != ShaderMap.end() ? it->second : nullptr;
}

void UShaderManager::ReleaseAll()
{
    for (auto& Pair : ShaderMap)
    {
        delete Pair.second;
    }
    ShaderMap.clear();
}
