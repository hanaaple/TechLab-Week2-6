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
    // 입력 레이아웃 정의 및 생성
    D3D11_INPUT_ELEMENT_DESC Layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (FAILED(Device->CreateInputLayout(Layout, ARRAYSIZE(Layout), VertexShaderBlob->GetBufferPointer(), VertexShaderBlob->GetBufferSize(), &InputLayout)))
    {
        std::cerr << "Failed to create input layout." << std::endl;
        VertexShaderBlob->Release();
        PixelShaderBlob->Release();
        return false;
    }
    
    D3D11_BUFFER_DESC ConstantVertexBufferDesc = {};
    ConstantVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    ConstantVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    ConstantVertexBufferDesc.ByteWidth = sizeof(VertexConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    ConstantVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정

    Device->CreateBuffer(&ConstantVertexBufferDesc, nullptr, &VertexConstantBuffer);

    D3D11_BUFFER_DESC ConstantPixelBufferDesc = {};
    ConstantPixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    ConstantPixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    ConstantPixelBufferDesc.ByteWidth = sizeof(PixelConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    ConstantPixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정

    Device->CreateBuffer(&ConstantPixelBufferDesc, nullptr, &PixelConstantBuffer);
    
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
    DeviceContext->IASetInputLayout(GetInputLayout());

}

void UShader::UpdateVertexConstantBuffer(ID3D11DeviceContext* DeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (SUCCEEDED(DeviceContext->Map(VertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
    {
        memcpy(MappedResource.pData, &VertexConstants, sizeof(FVertexConstants));
        DeviceContext->Unmap(VertexConstantBuffer, 0);
    }
    DeviceContext->VSSetConstantBuffers(0, 1, &VertexConstantBuffer);
}

void UShader::UpdatePixelConstantBuffer(ID3D11DeviceContext* DeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (SUCCEEDED(DeviceContext->Map(PixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
    {
        memcpy(MappedResource.pData, &PixelConstants, sizeof(FPixelConstants));
        DeviceContext->Unmap(PixelConstantBuffer, 0);
    }
    DeviceContext->PSSetConstantBuffers(0, 1, &PixelConstantBuffer);
}

void UShader::Release()
{
    if (VertexShader) { VertexShader->Release(); VertexShader = nullptr; }
    if (PixelShader) { PixelShader->Release(); PixelShader = nullptr; }
    if (VertexConstantBuffer) { VertexConstantBuffer->Release(); VertexConstantBuffer = nullptr; }
    if (PixelConstantBuffer) { PixelConstantBuffer->Release(); PixelConstantBuffer = nullptr; }
}
