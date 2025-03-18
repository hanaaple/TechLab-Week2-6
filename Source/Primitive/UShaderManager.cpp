#include "UShaderManager.h"

#include <filesystem>

#include "Core/Rendering/URenderer.h"
#include "Static/FEditorManager.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

void UShaderManager::Initialize(const URenderer& Renderer)
{
    Device = Renderer.GetDevice();
    DeviceContext = Renderer.GetDeviceContext();
}

void UShaderManager::LoadAllShaders()
{
    LoadShader(Device, L"Shaders/DefaultShader.hlsl");
    //LoadShader(Device, FName("PickingShader"), L"Shaders/ShaderW0.hlsl", "mainVS", "PickingPS");
    LoadShader(Device, L"Shaders/PickingShader.hlsl");
    LoadShader(Device, L"Shaders/OutlineShader.hlsl");
    LoadShader(Device, L"Shaders/TextShader.hlsl");
    LoadShader(Device, L"Shaders/CustomShader.hlsl",[](UPrimitiveComponent* PrimitiveComp)
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
            ACamera* Camera = FEditorManager::Get().GetCamera();
            ID3D11DeviceContext* DeviceContext = UShaderManager::Get().DeviceContext;
            FMatrix ViewMatrix = Camera->GetViewMatrix();
            FMatrix ProjectionMatrix;

            //카메라의 프로젝션 타입 확인
            float AspectRatio = UEngine::Get().GetScreenRatio();
            float FOV = FMath::DegreesToRadians(Camera->GetFieldOfView());
            float Near = Camera->GetNear();
            float Far = Camera->GetFar();

            if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
            {
                ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);
            }
            else if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
            {
                ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);

                // TODO: 추가 필요.
                // ProjectionMatrix = FMatrix::OrthoForLH(FOV, AspectRatio, Near, Far);
            }

            //MVP 행렬 계산
            FMatrix MVP = FMatrix::Transpose(
                PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);

            UShader* Shader = UShaderManager::Get().GetShader(FName("DefaultShader"));
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
    NewShader->UpdateConstantBufferFunction = UpdateConstantBufferFunction;
    ShaderMap[Name.GetDisplayIndex()] = NewShader;
    return NewShader;
}

UShader* UShaderManager::GetShader(const FName& Name)
{
    auto it = ShaderMap.Find(Name.GetDisplayIndex());
    return *it;
}
UShader* UShaderManager::GetShader(const EShaderType& Type)
{
    return GetShader(ShaderTypeToFName(Type));
}


void UShaderManager::ReleaseAll()
{
}
