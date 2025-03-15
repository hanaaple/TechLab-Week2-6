#include "FEditorManager.h"
#include "Object/World/World.h"
#include "Object/Gizmo/EditorGizmos.h"

void FEditorManager::SelectActor(AActor* NewActor)
{
    if (ControlGizmo == nullptr)
    {
    	ControlGizmo = UEngine::Get().GetWorld()->SpawnActor<AEditorGizmos>();
    	ControlGizmo->SetActorVisibility(false);
    }

	if (SelectedActor == NewActor)
		return;
	
    if (SelectedActor != nullptr && SelectedActor != NewActor)
    {
        SelectedActor->UnPick();
        ControlGizmo->SetActorVisibility(false);
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        SelectedActor->Pick();
        ControlGizmo->SetActorVisibility(true);
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
