#include "FEditorManager.h"
#include "Object/World/World.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Object/Actor/ABoundingBox.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"

void FEditorManager::SelectActor(AActor* NewActor)
{
    if (ControlGizmo == nullptr)
    {
    	ControlGizmo = UEngine::Get().GetWorld()->SpawnActor<AEditorGizmos>();
    	ControlGizmo->SetActorVisibility(false);
    }

    if (AABB == nullptr) {
        AABB = UEngine::Get().GetWorld()->SpawnActor<ABoundingBoxActor>();
        AABB->SetActorVisibility(true);
    }

	if (SelectedActor == NewActor)
		return;
	
    if (SelectedActor != nullptr && SelectedActor != NewActor)
    {
        SelectedActor->UnPick();
        AABB->SetActorVisibility(false);
        ControlGizmo->SetActorVisibility(false);
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        SelectedActor->Pick();
        ControlGizmo->SetActorVisibility(true);
        AABB->SetActorVisibility(true);
        //FVector Pos = SelectedActor->GetActorTransform().GetPosition();
		//FTransform GizmoTransform = GizmoHandle->GetActorTransform();
		//GizmoTransform.SetPosition(Pos);
		//GizmoHandle->SetActorTransform(GizmoTransform);
        //GizmoHandle
	}
}

void FEditorManager::SetCamera(ACamera* NewCamera)
{
    Camera = NewCamera;
}
