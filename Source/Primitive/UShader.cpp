#include "UShader.h"

#include <fstream>
#include <iostream>

#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

UShader::~UShader()
{
    Release();
}

bool UShader::LoadShader(ID3D11Device* Device, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry)
{
    ID3DBlob* VertexShaderBlob = nullptr;
    ID3DBlob* PixelShaderBlob = nullptr;
    ID3DBlob* ErrorMsg = nullptr;
    std::ifstream ShaderFile(FileName);
    if (!ShaderFile.good())
    {
        std::cerr << "Error: Shader file not found: "  << std::endl;
        return false;
    }
    if (FAILED(D3DCompileFromFile(FileName, nullptr, nullptr, *VertexEntry, "vs_5_0", 0, 0, &VertexShaderBlob, &ErrorMsg))) 
    {
        std::cerr << "Vertex Shader Compilation Failed" << std::endl;
        if (ErrorMsg) ErrorMsg->Release();
        return false;
    }
    if (FAILED(Device->CreateVertexShader(VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), nullptr, &VertexShader)))
    {
        VertexShaderBlob->Release();
        return false;
    }

    if (FAILED(D3DCompileFromFile(FileName, nullptr, nullptr, *PixelEntry, "ps_5_0", 0, 0, &PixelShaderBlob, &ErrorMsg)))
    {
        std::cerr << "Pixel Shader Compilation Failed" << std::endl;
        if (ErrorMsg) ErrorMsg->Release();
        return false;
    }
    if (FAILED(Device->CreatePixelShader(PixelShaderBlob->GetBufferPointer(), PixelShaderBlob->GetBufferSize(), nullptr, &PixelShader)))
    {
        PixelShaderBlob->Release();
        return false;
    }
    // 기본적인 Input Layout 생성
    D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    CreateInputLayout(Device, Layout, ARRAYSIZE(Layout), VertexShaderBlob);
    VertexShaderBlob->Release();
    PixelShaderBlob->Release();
    return true;
}
void UShader::CreateInputLayout(ID3D11Device* Device, const D3D11_INPUT_ELEMENT_DESC* LayoutDesc, UINT NumElements, ID3DBlob* VertexShaderBlob)
{
    if (InputLayout) InputLayout->Release();
    Device->CreateInputLayout(LayoutDesc, NumElements, VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), &InputLayout);
}
void UShader::Apply(ID3D11DeviceContext* DeviceContext) const
{
    DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    DeviceContext->IASetInputLayout(GetInputLayout());
    // InputLayout이 존재할 경우에만 적용
    if (InputLayout)
        DeviceContext->IASetInputLayout(InputLayout);
    
    for (const auto& Buffer : ConstantBuffers)
    {
        DeviceContext->VSSetConstantBuffers(Buffer.first, 1, &Buffer.second);
        DeviceContext->PSSetConstantBuffers(Buffer.first, 1, &Buffer.second);
    }
}
void UShader::UpdateConstantBuffer(UPrimitiveComponent* PrimitiveComp)
{
    if (UpdateConstantBufferFunction)
    {
        UpdateConstantBufferFunction(PrimitiveComp);
    }
}
void UShader::UpdateConstantBuffer(ID3D11DeviceContext* DeviceContext, uint32 BufferSlot, const void* Data, size_t DataSize)
{
    if (ConstantBuffers.find(BufferSlot) == ConstantBuffers.end()){
        ID3D11Device* Device;
        DeviceContext->GetDevice(&Device);
        CreateConstantBuffer(Device, BufferSlot, DataSize);
    }

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (SUCCEEDED(DeviceContext->Map(ConstantBuffers[BufferSlot], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
    {
        memcpy(MappedResource.pData, Data, DataSize);
        DeviceContext->Unmap(ConstantBuffers[BufferSlot], 0);
    }
}
void UShader::CreateConstantBuffer(ID3D11Device* Device, uint32 BufferSlot, size_t BufferSize)
{
    D3D11_BUFFER_DESC BufferDesc = {};
    BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    BufferDesc.ByteWidth = static_cast<UINT>((BufferSize + 0xf) & 0xfffffff0);
    BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    ID3D11Buffer* NewBuffer = nullptr;
    if (SUCCEEDED(Device->CreateBuffer(&BufferDesc, nullptr, &NewBuffer)))
    {
        ConstantBuffers[BufferSlot] = NewBuffer;
    }
}

void UShader::Release()
{
    if (VertexShader) { VertexShader->Release(); VertexShader = nullptr; }
    if (PixelShader) { PixelShader->Release(); PixelShader = nullptr; }
    if (InputLayout) { InputLayout->Release(); InputLayout = nullptr; }
    
    for (auto& Buffer : ConstantBuffers)
    {
        if (Buffer.second) { Buffer.second->Release(); }
    }
    ConstantBuffers.clear();
}
