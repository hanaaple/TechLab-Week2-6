#include "UShader.h"
#include <iostream>

UShader::~UShader()
{
    Release();
}

bool UShader::LoadShader(ID3D11Device* Device, const wchar_t* FileName, const char* VertexEntry, const char* PixelEntry)
{
    ID3DBlob* VertexShaderBlob = nullptr;
    ID3DBlob* PixelShaderBlob = nullptr;
    ID3DBlob* ErrorMsg = nullptr;
    
    if (FAILED(D3DCompileFromFile(FileName, nullptr, nullptr, VertexEntry, "vs_5_0", 0, 0, &VertexShaderBlob, &ErrorMsg)))
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
    
    if (FAILED(D3DCompileFromFile(FileName, nullptr, nullptr, PixelEntry, "ps_5_0", 0, 0, &PixelShaderBlob, &ErrorMsg)))
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
}

void UShader::Release()
{
    if (VertexShader) { VertexShader->Release(); VertexShader = nullptr; }
    if (PixelShader) { PixelShader->Release(); PixelShader = nullptr; }
}
