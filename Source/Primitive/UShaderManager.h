#pragma once
#define _TCHAR_DEFINED
#include "Core/Container/Map.h"
#include "UShader.h"

class URenderer;

class UShaderManager:public TSingleton<UShaderManager>
{
public:
    void Initialize(const URenderer& Renderer);
    
    UShader* LoadShader(ID3D11Device* Device, const wchar_t* FileName);
    UShader* LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry);
    UShader* GetShader(const FName& Name);
    void ReleaseAll();
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
private:
    TMap<int32, UShader*> ShaderMap;
};
