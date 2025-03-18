#pragma once
#include "UPrimitiveComponent.h"
class UCharComp : public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCharComp, Super)
public:
	UCharComp() : Super() {
		//bUseUV = true;
		SetTexture(ETextureType::FontTexture);
		SetMesh(EPrimitiveMeshType::EPT_Quad);
		SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		SetShaderType(EShaderType::TextShader);
	}
	char c;
};