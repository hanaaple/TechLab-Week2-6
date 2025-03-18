#include "UShaderManager.h"

#include <filesystem>

#include "Core/Rendering/URenderer.h"
#include "Object/PrimitiveComponent/CharComp.h"
#include "Static/FEditorManager.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

void UShaderManager::Initialize(const URenderer& Renderer)
{
    Device = Renderer.GetDevice();
    DeviceContext = Renderer.GetDeviceContext();
    this->Renderer = &Renderer;
}

void UShaderManager::LoadAllShaders()
{
    LoadShader(Device, L"Shaders/DefaultShader.hlsl");
    //LoadShader(Device, FName("PickingShader"), L"Shaders/ShaderW0.hlsl", "mainVS", "PickingPS");
    LoadShader(Device, L"Shaders/PickingShader.hlsl");
    LoadShader(Device, L"Shaders/OutlineShader.hlsl");
    LoadShader(Device, L"Shaders/TextShader.hlsl", [](UPrimitiveComponent* PrimitiveComp)
    {
        UShaderManager& ShaderManager = UShaderManager::Get();
        ID3D11Device* Device = ShaderManager.Device;
        ID3D11DeviceContext* DeviceContext = ShaderManager.DeviceContext;
        const URenderer* Renderer = ShaderManager.Renderer;
        Texture* Texture = UTextureLoader::Get().GetTexture(Renderer->GetCurrentTextureType(), Device, DeviceContext);

        if (Texture == nullptr)
        {
            return;
        }
        
        
        UShader* Shader = ShaderManager.GetShader(PrimitiveComp->GetShaderType());
        if (Shader)
        {
            
            if (PrimitiveComp->IsA<UCharComp>())
            {
                UCharComp* CharComp = static_cast<UCharComp*>(PrimitiveComp);
                auto info = Texture->GetCharInfoMap(CharComp->c);
                Shader->UpdateConstantBuffer(DeviceContext, 1, &info, sizeof(info));

                FMatrix ViewMatrix = Renderer->GetViewMatrix();
                FMatrix ProjectionMatrix = Renderer->GetProjectionMatrix();
                
                FMatrix BillboardMatrix = FMatrix::Transpose(CharComp->GetBillboardMatrix() * ViewMatrix * ProjectionMatrix);                                
                Shader->UpdateConstantBuffer(DeviceContext, 0, &BillboardMatrix, sizeof(BillboardMatrix));
            }
            else
            {
                FMatrix ViewMatrix = Renderer->GetViewMatrix();
                FMatrix ProjectionMatrix = Renderer->GetProjectionMatrix();

                // 빌보딩 여부에 따라 자체적으로 하게
                //MVP 행렬 계산
                FMatrix MVP = FMatrix::Transpose(PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);
                
                Shader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));
            }
        }
    });
    // LoadShader(Device, L"Shaders/TextureAtlasShader.hlsl", "mainVS", "mainPS", []()
    // {
    //     
    // });
    LoadShader(Device, L"Shaders/CustomShader.hlsl", [](UPrimitiveComponent* PrimitiveComp)
    {
    });
}

UShader* UShaderManager::LoadShader(ID3D11Device* Device, const wchar_t* FileName,
                                    std::function<void(UPrimitiveComponent*)> UpdateConstantBufferFunction)
{
    //파일 경로에서 파일명만 추출
    std::filesystem::path FilePath(FileName);
    FString ShaderNameStr = FilePath.stem().string(); // 확장자 제거한 파일명
    FName ShaderName(ShaderNameStr);

    return LoadShader(Device, ShaderName, FileName, "mainVS", "mainPS", UpdateConstantBufferFunction);
}

UShader* UShaderManager::LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName,
                                    const FString& VertexEntry, const FString& PixelEntry,
                                    std::function<void(UPrimitiveComponent*)> UpdateConstantBufferFunction)
{
    if (ShaderMap.Find(Name.GetDisplayIndex()) != nullptr)
        return ShaderMap[Name.GetDisplayIndex()];

    UShader* NewShader = new UShader();
    NewShader->SetShaderName(Name);
    if (!NewShader->LoadShader(Device, FileName, *VertexEntry, *PixelEntry)) // FString을 TCHAR*로 변환
    {
        delete NewShader;
        return nullptr;
    }
    //사용자가 무명 함수를 넘기지 않았을 경우, 기본 무명 함수를 적용
    if (!UpdateConstantBufferFunction)
    {
        UpdateConstantBufferFunction = [](UPrimitiveComponent* PrimitiveComp)
        {
            ID3D11DeviceContext* DeviceContext = UShaderManager::Get().DeviceContext;
            const URenderer* Renderer = UShaderManager::Get().Renderer;
            FMatrix ViewMatrix = Renderer->GetViewMatrix();
            FMatrix ProjectionMatrix = Renderer->GetProjectionMatrix();

            //MVP 행렬 계산
            FMatrix MVP = FMatrix::Transpose(
                PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);

            UShader* Shader = UShaderManager::Get().GetShader(EShaderType::DefaultShader);
            if (Shader)
            {
                Shader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));
                struct PSConstants
                {
                    FVector4 Color;
                    uint32 bUseVertexColor;
                };
                PSConstants PSData = {PrimitiveComp->GetCustomColor(), PrimitiveComp->IsUseVertexColor()};
                Shader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
            }
        };
    }
    //  무명 함수 설정
    NewShader->SetUpdateConstantBufferFunction(UpdateConstantBufferFunction);
    ShaderMap[Name.GetDisplayIndex()] = NewShader;
    return NewShader;
}

UShader* UShaderManager::GetShader(const EShaderType& Type)
{
    FName ShaderName = ShaderTypeToFName(Type);
    auto it = ShaderMap.Find(ShaderName.GetDisplayIndex());
    if (it == nullptr)
        return nullptr;
    return *it;
}


void UShaderManager::ReleaseAll()
{
}
