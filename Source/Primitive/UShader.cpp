#include "UShader.h"
#include <iostream>

UShader::~UShader()
{
    Release();
}

bool UShader::LoadShader(ID3D11Device* Device, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry)
{
    ID3DBlob* VertexShaderBlob = nullptr;
    ID3DBlob* PixelShaderBlob = nullptr;
    ID3DBlob* ErrorMsg = nullptr;

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

    VertexShaderBlob->Release();
    PixelShaderBlob->Release();
    return true;
}

void UShader::Apply(ID3D11DeviceContext* DeviceContext) const
{
    DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    
    if (VertexConstantBuffer)
        DeviceContext->VSSetConstantBuffers(0, 1, &VertexConstantBuffer);
    
    if (PixelConstantBuffer)
        DeviceContext->PSSetConstantBuffers(0, 1, &PixelConstantBuffer);
}

void UShader::UpdateVertexConstantBuffer(ID3D11DeviceContext* DeviceContext, const void* Data, size_t DataSize)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (SUCCEEDED(DeviceContext->Map(VertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
    {
        memcpy(MappedResource.pData, Data, DataSize);
        DeviceContext->Unmap(VertexConstantBuffer, 0);
    }
}

void UShader::UpdatePixelConstantBuffer(ID3D11DeviceContext* DeviceContext, const void* Data, size_t DataSize)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (SUCCEEDED(DeviceContext->Map(PixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
    {
        memcpy(MappedResource.pData, Data, DataSize);
        DeviceContext->Unmap(PixelConstantBuffer, 0);
    }
}

void UShader::Release()
{
    if (VertexShader) { VertexShader->Release(); VertexShader = nullptr; }
    if (PixelShader) { PixelShader->Release(); PixelShader = nullptr; }
    if (VertexConstantBuffer) { VertexConstantBuffer->Release(); VertexConstantBuffer = nullptr; }
    if (PixelConstantBuffer) { PixelConstantBuffer->Release(); PixelConstantBuffer = nullptr; }
}
