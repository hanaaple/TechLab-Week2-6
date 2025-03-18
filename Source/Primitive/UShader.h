#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <d3dcompiler.h>
#include <unordered_map>
#include "Core/Math/Vector4.h"
#include "Core/Name/FName.h"

class UShader
{
public:
    UShader() = default;
    ~UShader();

    bool LoadShader(ID3D11Device* Device, const wchar_t* FileName, const FString& VertexEntry,
                    const FString& PixelEntry);
    void Apply(ID3D11DeviceContext* DeviceContext) const;
    void Release();

    FName GetShaderName() const { return ShaderName; }

    //Data에 FMatrix를 바로 넣어도 되고 자료가 여러개라면 struct 형태로 삽입
    //만약 해당 버퍼 슬롯에 버퍼가 없다면 자동으로 생성
    void UpdateConstantBuffer(ID3D11DeviceContext* DeviceContext, uint32 BufferSlot, const void* Data, size_t DataSize);

    ID3D11VertexShader* GetVertexShader() const { return VertexShader; }
    ID3D11PixelShader* GetPixelShader() const { return PixelShader; }
    ID3D11InputLayout* GetInputLayout() const { return InputLayout; }

    //기본적으로 LoadShader에서 디폴트 InputLayout를 생성시킴.
    void CreateInputLayout(ID3D11Device* Device, const D3D11_INPUT_ELEMENT_DESC* LayoutDesc, UINT NumElements,
                           ID3DBlob* VertexShaderBlob);
    void CreateConstantBuffer(ID3D11Device* Device, uint32 BufferSlot, size_t BufferSize);

    void SetShaderName(const FName& Name) { ShaderName = Name; }

private:
    FName ShaderName;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;

    //키값은 buffer slot
    std::unordered_map<uint32, ID3D11Buffer*> ConstantBuffers; // 범용적인 ConstantBuffer 관리
};
