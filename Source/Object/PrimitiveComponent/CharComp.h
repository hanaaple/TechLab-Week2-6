#pragma once
#include "UPrimitiveComponent.h"
class UCharComp : public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCharComp, Super)
public:
	UCharComp() : Super() {
		bUseUV = true;
		SetTexture(UTextureLoader::Get().m_texture);
		SetMesh(EPrimitiveMeshType::EPT_Quad);
		SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	char c;
};