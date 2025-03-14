#include "ABoundingBox.h"
#include "Static/FEditorManager.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Core/Math/Vector.h"

ABoundingBoxActor::ABoundingBoxActor()
{
	bIsGizmo = true;

	UBoundingBoxComp* AABB = AddComponent<UBoundingBoxComp>();
	AABB->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	RootComponent = AABB;
	Max = FVector(0.5f, 0.5f, 0.5f);
	Min = FVector(-0.5f, -0.5f, -0.5f);
	SetActive(false);
}

void ABoundingBoxActor::Tick(float DeltaTime)
{
	UpdateTransform();
}

void ABoundingBoxActor::SetActive(bool bActive)
{
	bIsActive = bActive;
	boundingBoxComponent->SetCanBeRendered(bActive);
}

void ABoundingBoxActor::UpdateTransform()
{
	AActor* parentActor = FEditorManager::Get().GetSelectedActor();
	if (parentActor != nullptr && !parentActor->IsGizmoActor()) {
		Min = parentActor->GetRootComponent()->Children->aabb.Min;
		Max = parentActor->GetRootComponent()->Children->aabb.Max;
	}
	FTransform transform = RootComponent->GetComponentTransform();
	transform.SetScale(FVector(Max.X - Min.X, Max.Y - Min.Y, Max.Z - Min.Z));
	transform.SetRotation(FVector(0, 0, 0));
	RootComponent->SetRelativeTransform(transform);
}
