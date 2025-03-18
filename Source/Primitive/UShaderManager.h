#pragma once
#define _TCHAR_DEFINED
#include "Core/Container/Map.h"
#include "UShader.h"

class UShaderManager:public TSingleton<UShaderManager>
{
public:
    UShader* LoadShader(ID3D11Device* Device, const wchar_t* FileName);
    UShader* LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry);
    UShader* GetShader(const FName& Name);
    void ReleaseAll();
    
private:
    TMap<int32, UShader*> ShaderMap;
};
