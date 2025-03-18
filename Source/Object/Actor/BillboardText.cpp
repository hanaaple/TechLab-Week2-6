#include "BillBoardText.h"
#include <Object/PrimitiveComponent/TextComp.h>

ABillboardText::ABillboardText()
{
	bCanEverTick = true;

	UTextComp* TextComponent = AddComponent<UTextComp>();
	RootComponent = TextComponent;

	TextComp = TextComponent;
}

void ABillboardText::BeginPlay()
{
	Super::BeginPlay();
	TextComp->SetText("Hello World!");
	TextComp->SetText("Bye World!");
}

void ABillboardText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ABillboardText::GetTypeName()
{
	return "BillboardText";
}
