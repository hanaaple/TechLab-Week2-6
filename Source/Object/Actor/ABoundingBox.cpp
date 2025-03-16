#include "ABoundingBox.h"
#include "Static/FEditorManager.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Core/Math/Vector.h"

ABoundingBoxActor::ABoundingBoxActor()
{
	//bIsGizmo = true;

	UBoundingBoxComp* AABB = AddComponent<UBoundingBoxComp>();
	AABB->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	RootComponent = AABB;
	SetActorRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	Max = FVector(0.5f, 0.5f, 0.5f);
	Min = FVector(-0.5f, -0.5f, -0.5f);
	SetActorVisibility(true);
}

void ABoundingBoxActor::Tick(float DeltaTime)
{
	AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
	if (SelectedActor != nullptr && RootComponent)
	{
		UpdateTransform();
	}
	/*else if(SelectedActor == nullptr) {
		SetActorVisibility(false);
	}*/
	AActor::Tick(DeltaTime);
}

void ABoundingBoxActor::SetActorVisibility(bool bNewActive)
{
	if (RootComponent != nullptr) {
		RootComponent->SetVisibility(bNewActive);
	}
}


void ABoundingBoxActor::UpdateTransform()
{
	AActor* parentActor = FEditorManager::Get().GetSelectedActor();
	FVector position;
	if (parentActor != nullptr && !parentActor->IsGizmoActor()) {
		USceneComponent* rootComp = parentActor->GetRootComponent();
		UPrimitiveComponent* component = dynamic_cast<UPrimitiveComponent*>(rootComp);
		Min = component->aabb.Min;
		Max = component->aabb.Max;
		position = rootComp->GetComponentTransform().GetPosition();
		FTransform transform = RootComponent->GetComponentTransform();
		transform.SetPosition((Min + Max) / 2);
		transform.SetScale(FVector(Max.X - Min.X, Max.Y - Min.Y, Max.Z - Min.Z));
		transform.SetRotation(FVector(0, 0, 0));
		SetActorRelativeTransform(transform);
	}
}

const char* ABoundingBoxActor::GetTypeName()
{
	return "AABBActor";
}
