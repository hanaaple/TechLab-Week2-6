#include "BillBoardText.h"
#include <Object/PrimitiveComponent/TextComp.h>

ABillboardText::ABillboardText()
{
	bCanEverTick = true;

	
}

void ABillboardText::BeginPlay()
{
	Super::BeginPlay();
	UTextComp* TextComponent = AddComponent<UTextComp>();
	RootComponent = TextComponent;
	TextComponent->SetRelativeTransform(FTransform());

	TextComponent->SetText("Hello World!");
	TextComponent->SetText("Bye World!");
	//FIXME : testcode
	//text = "W";
	UTextureLoader::Get().DrawText("");
	//UTextureLoader::Get().DrawTextW(text); 
}

void ABillboardText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ABillboardText::GetTypeName()
{
	return "BillboardText";
}
