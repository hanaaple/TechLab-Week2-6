#include "AABBPicker.h"
#include "Core/Input/PlayerInput.h"
#include "Static/FEditorManager.h"
#include "Object/World/World.h"
#include "Object/Gizmo/EditorGizmos.h"

AAABBPicker::AAABBPicker()
{
	bIsGizmo = true; //월드 클리어시 삭제 방지
}

void AAABBPicker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAABBPicker::LateTick(float DeltaTime)
{
	AActor::LateTick(DeltaTime);

	if (APlayerInput::Get().GetMouseDown(false)) {
		FVector mousePos = APlayerInput::Get().GetMouseDownNDCPos(false);
		ACamera* camera = FEditorManager::Get().GetCamera();
		FVector rayOrigin = camera->GetActorTransform().GetPosition();
		FVector rayDir = RayCast(mousePos, camera);
		AActor* pickedActor = CheckCollision(rayOrigin, rayDir);
		if (pickedActor == nullptr) { 
			if(CheckGizmo(rayOrigin, rayDir) == nullptr)
			{
				FEditorManager::Get().SelectActor(nullptr);
			}
			return;
		}
		if (pickedActor->IsGizmoActor() == false) {
			if (pickedActor == FEditorManager::Get().GetSelectedActor())
			{
				return;
			}
			else
			{
				FEditorManager::Get().SelectActor(pickedActor);
				UE_LOG("Pick - UUID: %d", pickedActor->GetUUID());
			}
		}
	}
	if (APlayerInput::Get().IsPressedMouse(false)) {
		if (FEditorManager::Get().GetGizmoHandle()->GetSelectedAxis() == ESelectedAxis::None) {
			FVector mousePos = APlayerInput::Get().GetMouseDownNDCPos(false);
			ACamera* camera = FEditorManager::Get().GetCamera();
			FVector rayOrigin = camera->GetActorTransform().GetPosition();
			FVector rayDir = RayCast(mousePos, camera);
			UCylinderComp* pickedAxis = CheckGizmo(rayOrigin, rayDir);
			if (pickedAxis != nullptr) {
				if (FEditorManager::Get().GetGizmoHandle() != nullptr && FEditorManager::Get().GetSelectedActor() != nullptr) {
					AEditorGizmos* gizmo = FEditorManager::Get().GetGizmoHandle();
					gizmo->SetPrevMousePos(clickedPosition);
					FTransform AT = FEditorManager::Get().GetSelectedActor()->GetActorTransform();
					gizmo->SetActorXAxis(FVector4(AT.GetVisualForward().X, AT.GetVisualForward().Y, AT.GetVisualForward().Z, 1.0f));
					gizmo->SetActorYAxis(FVector4(AT.GetVisualRight().X, AT.GetVisualRight().Y, AT.GetVisualRight().Z, 1.0f));
					gizmo->SetActorZAxis(FVector4(AT.GetVisualUp().X, AT.GetVisualUp().Y, AT.GetVisualUp().Z, 1.0f));
				}
				ESelectedAxis selectedAxis = FEditorManager::Get().GetGizmoHandle()->IsAxis(pickedAxis);
			}
		}
	}
	else
	{
		if (AEditorGizmos* Handle = FEditorManager::Get().GetGizmoHandle())
		{
			Handle->SetSelectedAxis(ESelectedAxis::None);
		}
	}
}

const char* AAABBPicker::GetTypeName()
{
	return nullptr;
}

FVector AAABBPicker::RayCast(FVector mouse, ACamera* camera)
{
	FVector rayOrigin = camera->GetActorTransform().GetPosition();
	FVector4 ndc(mouse.X, mouse.Y, 1.0f, 1.0f);
	FMatrix inverseView = camera->GetViewMatrix().Inverse();
	UEngine::Get().GetRenderer()->UpdateProjectionMatrix(camera);
	FMatrix inverseProjection = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();

	FVector4 rayView = ndc * inverseProjection;
	if (rayView.W != 0) {
		rayView.X = rayView.X / rayView.W;
		rayView.Y = rayView.Y / rayView.W;
		rayView.Z = rayView.Z / rayView.W;
		rayView.W = 1.0f;
	}
	FVector4 rayWorld = rayView * inverseView;
	clickedPosition = rayWorld;
	FVector rayDir = FVector(rayWorld.X, rayWorld.Y, rayWorld.Z) - rayOrigin;
	rayDir.Normalize();
	return rayDir;
}

AActor* AAABBPicker::CheckCollision(FVector rayOrigin, FVector rayDir)
{
	UPrimitiveComponent* PickedComponent = nullptr;
	UWorld* world = UEngine::Get().GetWorld();
	TArray<UPrimitiveComponent*> components = world->GetRenderComponents();
	float dist = 10000;
	for (auto component : components) {
		if (component != nullptr && !component->GetOwner()->IsGizmoActor()) {
			if (component->GetVisibleFlag()) {
				FOBB boundingBox = component->obb;

				FVector center = boundingBox.Center;
				FVector localOrigin = center - rayOrigin;

				float tMin = -FLT_MAX;
				float tMax = FLT_MAX;

				for (int i = 0; i < 3; i++) {
					FVector axis = boundingBox.axis[i];
					float axisAlignedOrigin = axis.Dot(localOrigin);
					float axisAlignedDir = axis.Dot(rayDir);

					if (FMath::Abs(axisAlignedDir) > 1e-6f) {
						float t1 = (axisAlignedOrigin + boundingBox.halfSize[i]) / axisAlignedDir;
						float t2 = (axisAlignedOrigin - boundingBox.halfSize[i]) / axisAlignedDir;

						if (t1 > t2) {
							float temp = t1;
							t1 = t2;
							t2 = temp;
						}

						tMin = FMath::Max(tMin, t1);
						tMax = FMath::Min(tMax, t2);

						if (tMin > tMax || tMax < 0)
							break;
					}
					else if (-axisAlignedOrigin - boundingBox.halfSize[i] > 0 || -axisAlignedOrigin + boundingBox.halfSize[i] < 0)
					{
						break;
					}
				}
				if (tMin <= tMax && tMin >= 0 && component->GetOwner()->GetTypeName() != "Actor") {
					float objDist = (component->GetComponentTransform().GetPosition() - rayOrigin).Length();
					if (objDist < dist) {
						PickedComponent = component;
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

UCylinderComp* AAABBPicker::CheckGizmo(FVector rayOrigin, FVector rayDir)
{
	UCylinderComp* PickedComponent = nullptr;
	AEditorGizmos* gizmos = FEditorManager::Get().GetGizmoHandle();
	TArray<UCylinderComp*> components = gizmos->GetAxis();
	float dist = 10000;
	for (auto component : components) {
		if (component != nullptr) {
			if (component->GetVisibleFlag()) {
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
					if (component->GetDepth() > 0) {
						objDist = 0;
					}
					if (objDist < dist) {
						PickedComponent = component;
					}
				}
			}
		}
	}
	if (PickedComponent != nullptr) {
		return PickedComponent;
	}
	else {
		return nullptr;
	}
}
