#pragma once
#include <unordered_map>
#include "UShader.h"

class UShaderManager:TSingleton<UShaderManager>
{
public:
    
    UShader* LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName, const char* VertexEntry, const char* PixelEntry);
    UShader* GetShader(const FName& Name);
    void ReleaseAll();
    
private:
    std::unordered_map<FName, UShader*> ShaderMap;
};
