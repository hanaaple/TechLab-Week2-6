#include "FEditorManager.h"
#include "Object/World/World.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Object/Actor/ABoundingBox.h"
#include "Object/Gizmo/RotationGizmo.h"

void FEditorManager::SelectActor(AActor* NewActor)
{
    if (ControlGizmo == nullptr)
    {
    	ControlGizmo = UEngine::Get().GetWorld()->SpawnActor<AEditorGizmos>();
    	ControlGizmo->SetActorVisibility(false);
    }

    if (RotationGizmo == nullptr) {
        RotationGizmo = UEngine::Get().GetWorld()->SpawnActor<ARotationGizmo>();
        RotationGizmo->SetActorVisibility(false);
    }

    if (AABB == nullptr) {
        AABB = UEngine::Get().GetWorld()->SpawnActor<ABoundingBoxActor>();
        AABB->SetActorVisibility(false);
    }

	if (SelectedActor == NewActor)
		return;
	
    if (SelectedActor != nullptr && SelectedActor != NewActor)
    {
        SelectedActor->UnPick();
        AABB->SetActorVisibility(false);
        ControlGizmo->SetActorVisibility(false);
        RotationGizmo->SetActorVisibility(false);
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        SelectedActor->Pick();
        
        AABB->SetActorVisibility(true);
        if (GizmoType == EGizmoType::Rotate) {
            ControlGizmo->SetActorVisibility(false);
            RotationGizmo->SetActorVisibility(true);
        }
        else {
            ControlGizmo->SetActorVisibility(true);
            RotationGizmo->SetActorVisibility(false);
        }
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

void FEditorManager::SetGizmoType(EGizmoType newType)
{
    GizmoType = newType;
    if (SelectedActor != nullptr) {
        if (GizmoType == EGizmoType::Rotate) {
            ControlGizmo->SetActorVisibility(false);
            RotationGizmo->SetActorVisibility(true);
        }
        else {
            ControlGizmo->SetActorVisibility(true);
            RotationGizmo->SetActorVisibility(false);
        }
    }
}
