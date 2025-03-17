#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Core/Name/FName.h"

class UShader
{
public:
    UShader() = default;
    ~UShader();
    
    bool LoadShader(ID3D11Device* Device, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry);
    void Apply(ID3D11DeviceContext* DeviceContext) const;
    void Release();
    
    FName GetShaderName() const { return ShaderName; }
    void SetShaderName(const FName& Name) { ShaderName = Name; }

    void UpdateVertexConstantBuffer(ID3D11DeviceContext* DeviceContext, const void* Data, size_t DataSize);
    void UpdatePixelConstantBuffer(ID3D11DeviceContext* DeviceContext, const void* Data, size_t DataSize);

private:
    FName ShaderName;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    
    ID3D11Buffer* VertexConstantBuffer = nullptr;
    ID3D11Buffer* PixelConstantBuffer = nullptr;
};
