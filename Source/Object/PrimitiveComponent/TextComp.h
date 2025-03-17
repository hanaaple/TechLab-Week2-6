#include "UPrimitiveComponent.h"
#pragma once
class UTextComp: public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTextComp, Super)
public:
	UTextComp() : Super() {
		UTextureLoader::Get().LoadTexture("Resources/tempTexture.png");
		Texture = UTextureLoader::Get().m_texture;
		//bUseUV = true;
	}
	virtual ~UTextComp() = default;
	EPrimitiveMeshType GetMeshType() override {
		return EPrimitiveMeshType::EPT_Quad;
	}

	//string compText
	//void SetText() 글자별 Quad 추가
	//void SetFont()	
	// Tick Component() 에서 위치, 크기 조절.
	// UMaterialnstancedDynamic을 활용하여 텍스트 색상 변경.
};