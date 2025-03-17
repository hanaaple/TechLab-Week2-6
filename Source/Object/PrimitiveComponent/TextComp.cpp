#include "TextComp.h"
#include "Object/Actor/Actor.h"

void UTextComp::SetText(string NextText)
 {
	 for (UCharComp* Char : CharComponents) {
		 Owner->RemoveComponent(Char);
	 }
	 CharComponents.Empty();

	 // 새 텍스트 저장
	 Text = NextText;
	 //FVector BasePosition = GetRelativeTransform().GetPosition();		// UTextComp의 현재 위치

	 // 각문자에 대해 UCharComp 생성 및 배치
	 float YOffset = 0.0f;
	 for (int32 i = 0; i < Text.length(); i++) {
		 UCharComp* NewCharComp = Owner->AddComponent<UCharComp>();
		 CharComponents.Add(NewCharComp);
		 NewCharComp->c = Text[i];

		 NewCharComp->SetupAttachment(this);
		 FTransform Transform = FTransform();
		 // 문자 위치 설정(TextComp 기준 상대 위치)
		 FVector CharPosition = FVector(0, YOffset, 0);
		 Transform.SetPosition(CharPosition);
		 NewCharComp->SetRelativeTransform(Transform);

		 YOffset += CharSpacing;
	 }
	}