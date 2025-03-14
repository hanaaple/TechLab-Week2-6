#include "FEditorManager.h"
#include "Core/Engine.h"
#include "Object/World/World.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Object/Actor/ABoundingBox.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"


void FEditorManager::SelectActor(AActor* NewActor)
{
    if (GizmoHandle == nullptr)
    {
		GizmoHandle = UEngine::Get().GetWorld()->SpawnActor<AEditorGizmos>();
    	GizmoHandle->SetDepth(1);
        GizmoHandle->SetActorVisibility(false);
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
        GizmoHandle->SetActorVisibility(false);
        AABB->SetActorVisibility(false);
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        SelectedActor->Pick();
        GizmoHandle->SetActorVisibility(true);
        AABB->SetActorVisibility(true);
        //FVector Pos = SelectedActor->GetActorTransform().GetPosition();
		//FTransform GizmoTransform = GizmoHandle->GetActorTransform();
		//GizmoTransform.SetPosition(Pos);
		//GizmoHandle->SetActorTransform(GizmoTransform);
        //GizmoHandle
	}

    if (SelectedActor == nullptr) {
        AABB->SetActorVisibility(false);
    }
}

void FEditorManager::SetCamera(ACamera* NewCamera)
{
    Camera = NewCamera;
}
