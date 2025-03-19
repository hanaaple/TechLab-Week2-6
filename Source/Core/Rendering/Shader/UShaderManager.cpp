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
    LoadShader(Device, L"Shaders/DefaultShader.hlsl", [](FConstantBufferContext* ConstantContext)
        {
            UShaderManager& ShaderManager = UShaderManager::Get();
            
            UShader* Shader = ShaderManager.GetShader(EShaderType::DefaultShader);
            if (Shader == nullptr)
            {
                return;
            }
            ID3D11DeviceContext* DeviceContext = ShaderManager.DeviceContext;
            const URenderer* Renderer = ShaderManager.Renderer;
            FMatrix ViewMatrix = Renderer->GetViewMatrix();
            FMatrix ProjectionMatrix = Renderer->GetProjectionMatrix();

            UPrimitiveComponent* PrimitiveComp = ConstantContext->PrimitiveComponent;
            FMatrix MVP;
            if (PrimitiveComp != nullptr)
            {
                MVP = FMatrix::Transpose(PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);
            }
            else
            {
                MVP = FMatrix::Transpose(ViewMatrix * ProjectionMatrix);   
            }
            
            Shader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));
            
            if (PrimitiveComp != nullptr)
            {
                struct PSConstants
                {
                    FVector4 Color;
                    uint32 bUseVertexColor;
                };
                PSConstants PSData = {PrimitiveComp->GetCustomColor(), PrimitiveComp->IsUseVertexColor()};
                Shader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
            }
            else
            {
                struct PSConstants
                {
                    FVector4 Color;
                    uint32 bUseVertexColor;
                };
                PSConstants PSData = {FVector4(), true};
                Shader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
            }
        });
    LoadShader(Device, L"Shaders/TextShader.hlsl", [](FConstantBufferContext* ConstantContext)
    {
        // 특정 쉐이더는 
        UShaderManager& ShaderManager = UShaderManager::Get();
        
        UShader* Shader = ShaderManager.GetShader(EShaderType::TextureShader);        

        if (Shader == nullptr)
        {
            return;
        }

        ID3D11Device* Device = ShaderManager.Device;
        ID3D11DeviceContext* DeviceContext = ShaderManager.DeviceContext;
        const URenderer* Renderer = ShaderManager.Renderer;
        UPrimitiveComponent* PrimitiveComp = ConstantContext->PrimitiveComponent;

        FMatrix ViewMatrix = Renderer->GetViewMatrix();
        FMatrix ProjectionMatrix = Renderer->GetProjectionMatrix();
        
        FMatrix MVP;

        struct FUseUV
        {
            int bUseUV;
        };
        FUseUV UseUV;
        UseUV.bUseUV = 1;
        
        if (PrimitiveComp != nullptr)
        {
            Texture* Texture = UTextureLoader::Get().GetTexture(PrimitiveComp->GetTexture(), Device, DeviceContext);    
            // MVP 빌보드 여부
            if (PrimitiveComp->IsA<UCharComp>())
            {
                UCharComp* CharComp = static_cast<UCharComp*>(PrimitiveComp);
                MVP = FMatrix::Transpose(CharComp->GetBillboardMatrix() * ViewMatrix * ProjectionMatrix);
            }
            else
            {
                MVP = FMatrix::Transpose(PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);
            }

            // Texture 사용여부
            if (Texture != nullptr)
            {
                if (PrimitiveComp->IsA<UCharComp>())
                {
                    UCharComp* CharComp = static_cast<UCharComp*>(PrimitiveComp);
                    auto info = Texture->GetCharInfoMap(CharComp->c);
                    Shader->UpdateConstantBuffer(DeviceContext, 1, &info, sizeof(info));
                }
                else
                {
                    TextureInfo info;
                    info.u = 0;
                    info.v = 0;
                    info.width = 1;
                    info.height = 1;
                    // 전체 텍스처 출력
                    Shader->UpdateConstantBuffer(DeviceContext, 1, &info, sizeof(info));
                }
            }
            else
            {
                // 기본 Vertex Color로 실행
                UseUV.bUseUV = 0;
            }
        }
        else
        {
            MVP = FMatrix::Transpose(ViewMatrix * ProjectionMatrix);

            Texture* Texture = UTextureLoader::Get().GetTexture(ConstantContext->BatchContext->TextureType, Device, DeviceContext);
            // Texture 사용여부
            if (Texture != nullptr)
            {
                // 전체 보여주는거로
                TextureInfo info;
                info.u = 0;
                info.v = 0;
                info.width = 1;
                info.height = 1;
                // 전체 텍스처 출력
                Shader->UpdateConstantBuffer(DeviceContext, 1, &info, sizeof(info));
            }
            else
            {
                // 기본 Vertex Color로 실행
                UseUV.bUseUV = 0;
            }
        }

        Shader->UpdateConstantBuffer(DeviceContext, 2, &UseUV, sizeof(UseUV));
        Shader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));
    });
    LoadShader(Device,L"Shaders/PrimitiveShader.hlsl",[](FConstantBufferContext* ConstantContext)
    {
        UShaderManager& ShaderManager = UShaderManager::Get();

        UShader* Shader = ShaderManager.GetShader(EShaderType::PrimitiveShader);
        if (Shader == nullptr)
        {
            return;
        }
        
        ID3D11DeviceContext* DeviceContext = ShaderManager.DeviceContext;
        const URenderer *Renderer = ShaderManager.Renderer;
        UPrimitiveComponent* PrimitiveComp = ConstantContext->PrimitiveComponent;
        
        FMatrix ViewMatrix = Renderer->GetViewMatrix();
        FMatrix ProjectionMatrix = Renderer->GetProjectionMatrix();
        
        FMatrix MVP;
        if (PrimitiveComp != nullptr)
        {
            MVP = FMatrix::Transpose(ViewMatrix * ProjectionMatrix);
        }
        else
        {
            MVP = FMatrix::Transpose(PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);
        }
        Shader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));

        struct PSConstants
        {
            FVector4 Color;
            uint32 bUseVertexColor;
            float brightness;
        };

        if (PrimitiveComp != nullptr)
        {
            float brightness=1.0f;
            if (FEditorManager::Get().GetSelectedActor())
            {
                //std::cout<<FEditorManager::Get().GetSelectedActor()->GetUUID()<<" "<<PrimitiveComp->GetUUID()<<" "<<PrimitiveComp->GetOwner()->GetUUID()<<std::endl;
                if (FEditorManager::Get().GetSelectedActor()->GetUUID()==PrimitiveComp->GetOwner()->GetUUID())
                    brightness=2.0f;
                //brightness=2.0f;
            }
            PSConstants PSData = {PrimitiveComp->GetCustomColor(), PrimitiveComp->IsUseVertexColor(),brightness};
            Shader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
        }
        else
        {
            PSConstants PSData = {FVector4(), true, 1.f};
            Shader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
        }
    });
}

UShader* UShaderManager::LoadShader(ID3D11Device* Device, const wchar_t* FileName,
                                    std::function<void(FConstantBufferContext*)> UpdateConstantBufferFunction)
{
    //파일 경로에서 파일명만 추출
    std::filesystem::path FilePath(FileName);
    FString ShaderNameStr = FilePath.stem().string(); // 확장자 제거한 파일명
    FName ShaderName(ShaderNameStr);

    return LoadShader(Device, ShaderName, FileName, "mainVS", "mainPS", UpdateConstantBufferFunction);
}

UShader* UShaderManager::LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName,
                                    const FString& VertexEntry, const FString& PixelEntry,
                                    std::function<void(FConstantBufferContext*)> UpdateConstantBufferFunction)
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
        UpdateConstantBufferFunction = GetShader(EShaderType::DefaultShader)->GetUpdateConstantBufferFunction();
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
