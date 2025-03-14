#include "AABBPicker.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Math/Transform.h"
#include "Static/FEditorManager.h"
#include "Object/Actor/Camera.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/World/World.h"
#include "Object/Actor/ABoundingBox.h"

#include <windows.h>
#include <cstdio>

AAABBPicker::AAABBPicker()
{
	bIsGizmo = true; //월드 클리어시 삭제 방지
}

void AAABBPicker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#include <string>
void AAABBPicker::LateTick(float DeltaTime)
{
	AActor::LateTick(DeltaTime);

	if (APlayerInput::Get().GetMouseDown(false)) {
		FVector mousePos = APlayerInput::Get().GetMouseDownNDCPos(false);
		ACamera* camera = FEditorManager::Get().GetCamera();
		FVector rayOrigin = camera->GetActorTransform().GetPosition();
		FVector4 ndc(mousePos.X, mousePos.Y, 1.0f, 1.0f);
		FMatrix inverseView = camera->GetActorTransform().GetViewMatrix().Inverse();
		UEngine::Get().GetRenderer()->UpdateProjectionMatrix(camera);
		FMatrix inverseProjection = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();

		FVector4 rayView = inverseProjection * ndc;
		if (rayView.W != 0) {
			rayView.X = rayView.X / rayView.W;
			rayView.Y = rayView.Y / rayView.W;
			rayView.Z = rayView.Z / rayView.W;
			rayView.W = 1.0f;
		}
		FVector4 rayWorld = inverseView * rayView;
		FVector rayDir = FVector(rayWorld.X, rayWorld.Y, rayWorld.Z) - rayOrigin;
		rayDir.Normalize();
		AActor* pickedActor = CheckCollision(rayOrigin, rayDir);
		if (pickedActor == nullptr) {
			return;
		}
		if (pickedActor->IsGizmoActor() == false) {
			if (pickedActor == FEditorManager::Get().GetSelectedActor())
			{
				FEditorManager::Get().SelectActor(nullptr);
				FEditorManager::Get().GetAABB()->SetActorVisibility(false);
			}
			else
			{
				FEditorManager::Get().SelectActor(pickedActor);
				UE_LOG("Pick - UUID: %d", pickedActor->GetUUID());
			}
		}
	}

}

const char* AAABBPicker::GetTypeName()
{
	return nullptr;
}

AActor* AAABBPicker::CheckCollision(FVector rayOrigin, FVector rayDir)
{
	UPrimitiveComponent* PickedComponent = nullptr;
	UWorld* world = UEngine::Get().GetWorld();
	TMap<EPrimitiveMeshType, TArray<UPrimitiveComponent*>> components = world->GetRenderComponents();
	if (PickedComponent == nullptr) {
		UE_LOG("nullptr init");
	}
	float dist = 10000;
	for (auto& [MeshType, RenderComponents] : components) {
		for (auto component : RenderComponents) {
			if (component != nullptr && component->GetOwner()->GetTypeName() != "AABBActor") {
				if (component != nullptr) {
					FAABB boundingBox = component->aabb;

					FVector center = (boundingBox.Max + boundingBox.Min) / 2.0f;
					float t1 = (boundingBox.Min.X - rayOrigin.X) / rayDir.X;
					float t2 = (boundingBox.Max.X - rayOrigin.X) / rayDir.X;

					float t3 = (boundingBox.Min.Y - rayOrigin.Y) / rayDir.Y;
					float t4 = (boundingBox.Max.Y - rayOrigin.Y) / rayDir.Y;

					float t5 = (boundingBox.Min.Z - rayOrigin.Z) / rayDir.Z;
					float t6 = (boundingBox.Max.Z - rayOrigin.Z) / rayDir.Z;

					float tMax = FMath::Min(FMath::Max(t1, t2), FMath::Max(t3, t4));
					tMax = FMath::Min(tMax, FMath::Max(t5, t6));

					float tMin = FMath::Max(FMath::Min(t1, t2), FMath::Min(t3, t4));
					tMin = FMath::Max(tMin, FMath::Min(t5, t6));

					if (tMax >= tMin && tMax > 0) {
						float objDist = FVector::Distance(center, rayOrigin);
						if (objDist < dist) {
							if (component->GetOwner()->GetTypeName() != "Actor") {
								PickedComponent = component;
								UE_LOG(PickedComponent->GetOwner()->GetTypeName());
							}
						}
					}
				}
			}
		}
	}
	if (PickedComponent != nullptr) {
		return PickedComponent->GetOwner();
	}
	else {
		return nullptr;
	}
}
