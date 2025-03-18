#include "TextComp.h"
#include "Object/Actor/Actor.h"

// FIXME : 추후 풀링으로 변경.
void UTextComp::SetText(string NextText)
 {
	int idx = 0;
	Text = NextText;
	for (idx; idx < CharComponents.Len(); idx++) {
		CharComponents[idx]->c = Text[idx];

		// 새로운 문자열이 이전 길이보다 작으면 컴포넌트 삭제
		if (idx == NextText.length() - 1) {
			idx += 1;
			RemoveLongerCharComp(idx, CharComponents.Len());
			break;
		}
	}
	CreateAdditionCharComp(idx, Text.length());
}

void UTextComp::RemoveLongerCharComp(int& curIdx, int PrevCharCompLen)
{
	for (curIdx; curIdx < CharComponents.Len(); curIdx++) {
		Owner->RemoveComponent(CharComponents[curIdx]);
	}
}

void UTextComp::CreateAdditionCharComp(int& curIdx, int TextLen) {
	float YOffset = curIdx * CharSpacing;
	for (curIdx; curIdx < TextLen; curIdx++) {
		UCharComp* NewCharComp = Owner->AddComponent<UCharComp>();
		CharComponents.Add(NewCharComp);
		NewCharComp->c = Text[curIdx];
		NewCharComp->SetupAttachment(this);
		FTransform Transform = FTransform();
		// 문자 위치 설정(TextComp 기준 상대 위치)
		FVector CharPosition = FVector(0, YOffset, 0);
		Transform.SetPosition(CharPosition);
		NewCharComp->SetRelativeTransform(Transform);

		YOffset += CharSpacing;
	}
}