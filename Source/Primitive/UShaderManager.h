#pragma once
#define _TCHAR_DEFINED
#include "Core/Container/Map.h"
#include "UShader.h"

class URenderer;
enum class EShaderType : uint8
{
    DefaultShader,
    PickingShader,
    OutlineShader,
    TextShader,
    CustomShader,  // 이후 추가될 사용자 정의 쉐이더
};
//EShaderType -> FName 변환 함수
inline FName ShaderTypeToFName(EShaderType ShaderType)
{
    switch (ShaderType)
    {
    case EShaderType::DefaultShader:  return FName("DefaultShader");
    case EShaderType::PickingShader:  return FName("PickingShader");
    case EShaderType::OutlineShader:  return FName("OutlineShader");
    case EShaderType::TextShader:     return FName("TextShader");
    case EShaderType::CustomShader:   return FName("CustomShader");
    default:                          return FName("UnknownShader");
    }
}
class UShaderManager : public TSingleton<UShaderManager>
{
public:
    void Initialize(const URenderer& Renderer);

    void LoadAllShaders();
    //UShader* LoadShader(ID3D11Device* Device, const wchar_t* FileName);
    UShader* LoadShader(ID3D11Device* Device, const wchar_t* FileName,
                        std::function<void(UPrimitiveComponent*)> UpdateConstantBufferFunction=nullptr);
    UShader* LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName,
                        const FString& VertexEntry, const FString& PixelEntry,
                        std::function<void(UPrimitiveComponent*)> UpdateConstantBufferFunction=nullptr);
    UShader* GetShader(const FName& Name);
    UShader* GetShader(const EShaderType& Type);
    void ReleaseAll();
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;

private:
    TMap<int32, UShader*> ShaderMap;
};

