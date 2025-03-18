#pragma once
#include "UPrimitiveComponent.h"
#include "CharComp.h"
#pragma once

class UTextComp: public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTextComp, Super)

public:
	UTextComp() : Super() {
		bUseUV = true;
	}
	virtual ~UTextComp() = default;

	void SetText(FName NewText);

	void SetTextSpace(float NewTextSpace);
	
private:
	void UpdateCharacterChildren();

private:	
	TArray<UCharComp*> CharComponents;		// 개별 문자 컴포넌트 저장
	FName Text = FName("");							// 현재 텍스트 저장
	float CharSpacing = 0.5f;
};