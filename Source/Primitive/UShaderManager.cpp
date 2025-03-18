#include "UShaderManager.h"

#include <filesystem>

#include "Core/Rendering/URenderer.h"
#include "Static/FEditorManager.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
void UShaderManager::Initialize(const URenderer& Renderer){
    Device=Renderer.GetDevice();
    DeviceContext=Renderer.GetDeviceContext();
}
UShader* UShaderManager::LoadShader(ID3D11Device* Device, const wchar_t* FileName)
{
    //파일 경로에서 파일명만 추출
    std::filesystem::path FilePath(FileName);
    FString ShaderNameStr = FilePath.stem().string(); // 확장자 제거한 파일명
    FName ShaderName(ShaderNameStr);
    
    return LoadShader(Device, ShaderName, FileName, "mainVS", "mainPS");
}
UShader* UShaderManager::LoadShader(ID3D11Device* Device, const FName& Name, const wchar_t* FileName, const FString& VertexEntry, const FString& PixelEntry)
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
    //  무명 함수 설정
    NewShader->UpdateConstantBufferFunction = [NewShader](UPrimitiveComponent* PrimitiveComp)
    {
        /*
        ConstantUpdateInfo UpdateInfo{
            PrimitiveComp->GetComponentTransform(),
            PrimitiveComp->GetCustomColor(),
            PrimitiveComp->IsUseVertexColor(),
            PrimitiveComp->IsUseTexture()
        };*/
        ACamera *Camera = FEditorManager::Get().GetCamera();
        ID3D11DeviceContext* DeviceContext = Get().DeviceContext;
        FMatrix ViewMatrix = Camera->GetViewMatrix();
        FMatrix ProjectionMatrix;
        {
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
        }
        FMatrix MVP = FMatrix::Transpose(PrimitiveComp->GetComponentTransform().GetMatrix() * ViewMatrix * ProjectionMatrix);
        NewShader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));

        struct PSConstants { FVector4 Color; uint32 bUseVertexColor; };
        PSConstants PSData = { PrimitiveComp->GetCustomColor(), PrimitiveComp->IsUseVertexColor() };
        NewShader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
    };
    ShaderMap[Name.GetDisplayIndex()] = NewShader;
    return NewShader;
}

UShader* UShaderManager::GetShader(const FName& Name)
{
    auto it = ShaderMap.Find(Name.GetDisplayIndex());
    return *it;
}

void UShaderManager::ReleaseAll()
{

}
