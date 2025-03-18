#include "TextComp.h"
#include "Object/Actor/Actor.h"

void UTextComp::SetText(FName NewText)
{
	const int32 PrevLen = Text.Length();

	const int32 NewLen = NewText.Length();

	const int32 diffLen = NewLen - PrevLen;

	if (diffLen > 0)
	{
		for (int32 i = 0; i < diffLen; i++)
		{
			auto* Comp = GetOwner()->AddComponent<UCharComp>();
			CharComponents.Add(Comp);
			// Add Children?
		}
	}
	else if (diffLen < 0)
	{
		for (int32 i = 0; i < -diffLen; i++)
		{
			auto* Comp = CharComponents[0];
			CharComponents.Remove(Comp);
			GetOwner()->RemoveComponent(Comp);
			// Remove Children?
		}
	}
	
	Text = NewText;

	UpdateCharacterChildren();
}

void UTextComp::SetTextSpace(float NewTextSpace)
{
	CharSpacing = NewTextSpace;

	UpdateCharacterChildren();
}

void UTextComp::UpdateCharacterChildren()
{
	float YOffset = 0;
	
	for (int32 idx = 0; idx < Text.Length(); idx++)
	{
		UCharComp* CharComp = CharComponents[idx]; 
		CharComp->c = Text.ToStdString()[idx];
		
		FTransform Transform = FTransform();
		Transform.SetPosition({0, YOffset, 0});
		CharComp->SetRelativeTransform(Transform);
		YOffset += CharSpacing;
	}
}


void UTextComp::Activate()
{
}