#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Core/Math/Vector4.h"
#include "Core/Name/FName.h"
struct FVertexConstants
{
    FMatrix MVP;
    FVector4 Color;
    // true인 경우 Vertex Color를 사용하고, false인 경우 Color를 사용합니다.
    uint32 bUseVertexColor;
    uint32 bUseUV;//분리하면 필요 없어짐
};
struct FPixelConstants
{
    FMatrix MVP;
    FVector4 Color;
    // true인 경우 Vertex Color를 사용하고, false인 경우 Color를 사용합니다.
    uint32 bUseVertexColor;
    uint32 bUseUV;
};
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

    void UpdateVertexConstantBuffer(ID3D11DeviceContext* DeviceContext);
    void UpdatePixelConstantBuffer(ID3D11DeviceContext* DeviceContext);
    ID3D11VertexShader* GetVertexShader() const { return VertexShader; }
    ID3D11PixelShader* GetPixelShader() const { return PixelShader; }
    ID3D11InputLayout* GetInputLayout() const { return InputLayout; }

    
    FVertexConstants VertexConstants;
    FPixelConstants PixelConstants;
private:
    FName ShaderName;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    
    ID3D11Buffer* VertexConstantBuffer = nullptr;
    ID3D11Buffer* PixelConstantBuffer = nullptr;

};
