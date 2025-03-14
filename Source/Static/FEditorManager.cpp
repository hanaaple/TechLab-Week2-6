﻿#include "FEditorManager.h"
#include "Core/Engine.h"
#include "Object/World/World.h"
#include "Object/Gizmo/GizmoHandle.h"
#include "Object/Actor/ABoundingBox.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"

void FEditorManager::SelectActor(AActor* NewActor)
{
    if (GizmoHandle == nullptr)
    {
		GizmoHandle = UEngine::Get().GetWorld()->SpawnActor<AGizmoHandle>();
    	GizmoHandle->SetDepth(1);
        GizmoHandle->SetActive(false);
    }

    if (AABB == nullptr) {
        AABB = UEngine::Get().GetWorld()->SpawnActor<ABoundingBoxActor>();
        AABB->SetActive(false);
    }

	if (SelectedActor == NewActor)
		return;
	
    if (SelectedActor != nullptr && SelectedActor != NewActor)
    {
        SelectedActor->UnPick();
        GizmoHandle->SetActive(false);
        AABB->SetActive(false);
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        SelectedActor->Pick();
        GizmoHandle->SetActive(true);
        AABB->SetActive(true);
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
