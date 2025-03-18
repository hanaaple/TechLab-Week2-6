#pragma once
#include "Core/Math/Transform.h"
#include "Object/ActorComponent/ActorComponent.h"


class USceneComponent : public UActorComponent
{
	using Super = UActorComponent;
	using EAttachmentRule = EEndPlayReason::EAttachmentRule;
	friend class AActor;
    DECLARE_OBJECT(USceneComponent,Super)
public:
	USceneComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	// Get Set
public:
	void SetVisibility(bool bNewVisibility);
	bool GetVisibleFlag() const	{ return bVisible; }
	void Pick(bool bPicked);
	
	// Transform
public:
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

public:
	/* SnapToTarget을 제외하여 KeepRelative, KeepWorld만 구현 */
	void SetupAttachment(USceneComponent* InParent, EAttachmentRule AttachmentRule = EAttachmentRule::KeepWorld);
	void RemoveChild(USceneComponent* Child);
	const TArray<USceneComponent*>& GetAttachChildren() const { return AttachChildren; }
	
private:
	void UpdateChildTransforms();
	void UpdateComponentToWorld();	// Relative 기반 World Update
	void UpdateRelativeTransform();	// World 기반 Relative Update
	virtual void OnTransformation();
private:
	
	USceneComponent* AttachParent = nullptr;
	TArray<USceneComponent*> AttachChildren;

	/* Component's Relative Transform */
	FTransform RelativeTransform = FTransform();
	/* Component's World Transform */
	FTransform ComponentToWorld = FTransform();
	
private:
	bool bVisible = true;
	
	// debug
protected:
	bool bIsPicked = false;
};