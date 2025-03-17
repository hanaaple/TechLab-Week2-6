#include "UPrimitiveComponent.h"
#include "CharComp.h"
#pragma once

class UTextComp: public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTextComp, Super)

public:
	TArray<UCharComp*> CharComponents;		// 개별 문자 컴포넌트 저장
	string Text;							// 현재 텍스트 저장
	float CharSpacing = 0.5f;

	UTextComp() : Super() {
		bUseUV = true;
		UTextureLoader::Get().LoadTexture("Resources/tempTexture.png");
	}
	virtual ~UTextComp() = default;

	void SetText(string NewText);
};