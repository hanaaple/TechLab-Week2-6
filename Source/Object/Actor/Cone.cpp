#include "Cone.h"

#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

ACone::ACone()
{
    bCanEverTick = true;

    UConeComp* ConeComponent = AddComponent<UConeComp>();
    RootComponent = ConeComponent;
	
    SetActorRelativeTransform(FTransform());
}

void ACone::BeginPlay()
{
    Super::BeginPlay();
}

void ACone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UPrimitiveComponent* root = dynamic_cast<UPrimitiveComponent*>(RootComponent);
    TArray<FVertexSimple> vertices = root->GetVertexData();
    FVector Min = FVector(1000, 1000, 1000);
    FVector Max = -Min;
    for (FVertexSimple& vertex : vertices) {
        Min.X = FMath::Min(Min.X, vertex.X);
        Min.Y = FMath::Min(Min.Y, vertex.Y);
        Min.Z = FMath::Min(Min.Z, vertex.Z);
        Max.X = FMath::Max(Max.X, vertex.X);
        Max.Y = FMath::Max(Max.Y, vertex.Y);
        Max.Z = FMath::Max(Max.Z, vertex.Z);
    }
}

const char* ACone::GetTypeName()
{
    return "Cone";
}
