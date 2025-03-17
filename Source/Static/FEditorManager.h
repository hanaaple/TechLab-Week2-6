#pragma once
#include "Object/Actor/Actor.h"
#include "Object/Actor/Camera.h"

class AEditorGizmos;

class ABoundingBoxActor;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

    AEditorGizmos* GetGizmoHandle() const {return ControlGizmo;}
    
    ABoundingBoxActor* GetAABB() const { return AABB; }
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    AEditorGizmos* GizmoHandle = nullptr;
    ABoundingBoxActor* AABB = nullptr;
    AEditorGizmos* ControlGizmo = nullptr;
};
