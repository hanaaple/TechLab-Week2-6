#include "BillBoardText.h"
#include <Object/PrimitiveComponent/TextComp.h>

ABillboardText::ABillboardText()
{
	bCanEverTick = true;

	UTextComp* TextComponent = AddComponent<UTextComp>();
	RootComponent = TextComponent;
	TextComponent->SetRelativeTransform(FTransform());

	TextComponent->SetText("Hello World!");
	
	//FIXME : testcode
	//text = "W";
	UTextureLoader::Get().DrawText("");
	//UTextureLoader::Get().DrawTextW(text); 
}

void ABillboardText::BeginPlay()
{
	Super::BeginPlay();
}

void ABillboardText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ABillboardText::GetTypeName()
{
	return "BillboardText";
}
