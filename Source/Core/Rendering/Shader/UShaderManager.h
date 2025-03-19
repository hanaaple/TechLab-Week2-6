#pragma once
#include "ConstantBufferContext.h"
#include "Core/Container/Map.h"
#include "UShader.h"
#include "DataTypes/ShdaerType.h"

class URenderer;

//EShaderType -> FName 변환 함수
inline FName ShaderTypeToFName(EShaderType ShaderType)
{
    switch (ShaderType)
    {
    case EShaderType::DefaultShader: return FName("DefaultShader");
    case EShaderType::TextureShader: return FName("TextShader");
    case EShaderType::PrimitiveShader: return FName("PrimitiveShader");
    default: return FName("UnknownShader");
    }
}
class UShaderManager : public TSingleton<UShaderManager>
{
public:
    void Initialize(const URenderer& Renderer);

    void LoadAllShaders();
    
    //UShader* LoadShader(ID3D11Device* Device, const wchar_t* FileName);
    UShader* LoadShader(ID3D11Device* Device, const wchar_t* FileName,
                        std::function<void(FConstantBufferContext*)> UpdateConstantBufferFunction = nullptr);
    UShader* LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName,
                        const FString& VertexEntry, const FString& PixelEntry,
                        std::function<void(FConstantBufferContext*)> UpdateConstantBufferFunction=nullptr);

    UShader* GetShader(const EShaderType& Type);

    void ReleaseAll();

private:
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    const URenderer* Renderer;

private:
    TMap<int32, UShader*> ShaderMap;
};

