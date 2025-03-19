#include "BillBoardText.h"
#include <Object/PrimitiveComponent/TextComp.h>

#include "Object/World/World.h"

ABillboardText::ABillboardText()
{
	bIsGizmo = true;
	bCanEverTick = true;

	UTextComp* TextComponent = AddComponent<UTextComp>();
	RootComponent = TextComponent;

	TextComp = TextComponent;
}

void ABillboardText::BeginPlay()
{
	Super::BeginPlay();
}

void ABillboardText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FollowComponent != nullptr && !FollowComponent->GetIsActive())
	{
		auto* world = GetWorld();
		GetWorld()->DestroyActor(this);
		//delete FollowComponent;
		FollowComponent = nullptr;
	}
	else if (FollowComponent != nullptr)
	{
		FTransform Transform = GetActorTransform();
		Transform.SetPosition(FollowComponent->GetComponentTransform().GetPosition() + Offset);
		SetActorTransform(Transform);
	}
}

const char* ABillboardText::GetTypeName()
{
	return "BillboardText";
}

void ABillboardText::SetText(FName NewText)
{
	TextComp->SetText(NewText);
}
