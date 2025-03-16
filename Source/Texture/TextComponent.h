#pragma once
#include <Core/Math/Vector.h>
#include <Texture/TextComponent.h>
#include <Core/AbstractClass/Singleton.h>
#include "Object/USceneComponent.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include <d3d11.h>
#include "TextureLoader.h"
#include "Object/Actor/Actor.h"
// 실제로 문자가 그려질 사각형을 만드는 데 필요한 정점 데이터를 저장.
struct VertexType {
	FVector position;
	FVector2 texture;
};
class UTextComponent : public UPrimitiveComponent {
	using Super = USceneComponent;
public:
	UTextComponent();
	virtual ~UTextComponent() = default;

public:
	std::string text;

	//std::unique_ptr<UTextureLoader> textureLoader;
	//UEngine EngineInstance;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void DrawText(float x, float y);
	//void Shutdown();
	//void Render();
};