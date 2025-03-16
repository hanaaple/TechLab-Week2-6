#include "TextComponent.h"

UTextComponent::UTextComponent()
{
}

void UTextComponent::BeginPlay()
{
	Super::BeginPlay();

	UTextComponent* textComp = GetOwner()->AddComponent<UTextComponent>();
	textComp->SetupAttachment(this);

	// 폰트 텍스처 로드
	if (!UTextureLoader::Get().LoadTexture("Resources/tempTexture.png")) {
		MessageBoxW(NULL, L"Failed to load font texture!", L"Error", MB_OK | MB_ICONERROR);
	}
}

void UTextComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UTextComponent::DrawText(float x, float y)
{
	UTextureLoader::Get().DrawText(text);
}