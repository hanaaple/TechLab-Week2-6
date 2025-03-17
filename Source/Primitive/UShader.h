#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Core/Name/FName.h"

class UShader
{
public:
    UShader() = default;
    ~UShader();
    
    bool LoadShader(ID3D11Device* Device, const wchar_t* FileName, const char* VertexEntry, const char* PixelEntry);
    void Apply(ID3D11DeviceContext* DeviceContext) const;
    void Release();
    
    FName GetShaderName() const { return ShaderName; }
    void SetShaderName(const FName& Name) { ShaderName = Name; }
    
private:
    FName ShaderName;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
};
