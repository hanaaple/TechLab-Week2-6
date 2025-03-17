#include "UPrimitiveComponent.h"
#include "CharComp.h"
#pragma once

class UTextComp: public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTextComp, Super)

public:
	TArray<UCharComp*> CharComponents;		// 개별 문자 컴포넌트 저장
	string Text;							// 현재 텍스트 저장
	float CharSpacing = 1.0f;

	UTextComp() : Super() {
		bUseUV = true;
		UTextureLoader::Get().LoadTexture("Resources/tempTexture.png");
	}
	virtual ~UTextComp() = default;

	/* 텍스트 설정 함수 */
	void SetText(string NewText);
	//string compText
	//void SetText() 글자별 Quad 추가
	//void SetFont()	
	// Tick Component() 에서 위치, 크기 조절.
	// UMaterialnstancedDynamic을 활용하여 텍스트 색상 변경.
};