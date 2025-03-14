#pragma once
#include "UObject.h"
#include "Core/Math/Vector.h"
#include "Core/Container/Set.h"
#include "Core/Math/Transform.h"
#include "Core/Math/Matrix.h"
#include "Object/ActorComponent/ActorComponent.h"


class USceneComponent : public UActorComponent
{
	friend class AActor;
	using Super = UActorComponent;
	using EAttachmentRule = EEndPlayReason::EAttachmentRule;
public:
	USceneComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	/**
	 * @return Component's Relative(Local) Transform 
	 */	
	FTransform GetRelativeTransform() const;

	/**
	* @return Component's World Transform
	 */
	const FTransform& GetComponentTransform() const;

	void SetRelativeTransform(const FTransform& NewRelativeTransform);
	void SetWorldTransform(const FTransform& NewTransform);

	void Pick(bool bPicked);
	bool IsPicked() const { return bIsPicked; }

public:
	/* SnapToTarget을 제외하여 KeepRelative, KeepWorld만 구현 */
	void SetupAttachment(USceneComponent* InParent, EAttachmentRule AttachmentRule = EAttachmentRule::KeepWorld);
	// const TArray<USceneComponent*>& GetAttachChildren() const { return AttachChildren; }
	
private:
	// 부모의 월드 트랜스폼을 받아서 자신의 로컬 트랜스폼을 갱신, 부모를 바꿀 때 사용
	void ApplyParentWorldTransform();
	void UpdateChildTransforms();
	void UpdateComponentToWorld();
protected:
	void SetVisibility(bool bNewVisibility) const;
	bool GetVisibleFlag() const	{ return bVisible; }
	bool bCanEverTick = true;
	
private:
	// (Relative Transform 변경시?)
	// UpdateComponentToWorld  (매개변수) ->
	// UpdateComponentToWorldWithParent   (ComponentToWorld = NewWorld) ->
	// PropagateTransformUpdate ->
	// 1. OnUpdateTransform
	// 2. UpdateChildTransforms -> UpdateComponentToWorld



	// 독립적? (World Transform 변경시?)
	// SetWorldTransform -> Update RelativeTransform -> UpdateComponentToWorld?w
	


	// 컴포넌트가 추가되는 경우? -> Update World(확인용?) -> Child Init(Transform, Child.SetParnet)
	// ApplyToComponent? -> UpdateComponentToWorld ->
	// 1. Child.SetRelativeTransform
	// 2. Child.AttachToComponent, 
	// AttachToComponent?  (컴포넌트를 붙일떄 사용하는?)
	// -> SetRelative
	// -> ?? SetUpAttachment


	// Relative 변경 시
	// -> ComponentToWorld Update 해줘야됨. (혹은 매번 Parent recursive하게 구하던가)
	// -> Relative 바로 넣어주면 됨
	// -> 자식 Components Update 해줘야됨.

	// World 변경 시
	// -> ComponentToWorld = CalcNewComponentToWorld(RelativeTransform, Parent, SocketName); ????
	// -> Relative -> Parent와 상대적 Transform을 구해서 넣어줘야됨.		NewTransform.GetRelativeTransform(ParentToWorld);   
	// -> 자식 Components Update  
	
	// BoundBox를 각 SceneComponent에 대해 하여 최종 min, max를 Update?
	// UpdateBounds
	// FTransformUpdated TransformUpdated;??
	
	USceneComponent* AttachParent = nullptr;
	TArray<USceneComponent*> AttachChildren;

	/* Component's Relative Transform */
	FTransform RelativeTransform = FTransform();
	/* Component's World Transform */
	FTransform ComponentToWorld;
	
private:
	bool bVisible = true;
	// debug
protected:
	bool bIsPicked = false;
};