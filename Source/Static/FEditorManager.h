#pragma once
#include "Object/Actor/Actor.h"

class AEditorGizmos;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

    AEditorGizmos* GetGizmoHandle() const {return GizmoHandle;}
    
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    AEditorGizmos* GizmoHandle = nullptr;
};
