﻿#include "USceneComponent.h"
#include "Debug/DebugConsole.h"
#include "PrimitiveComponent/UPrimitiveComponent.h"

void USceneComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USceneComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FTransform USceneComponent::GetRelativeTransform() const
{
	//const FTransform RelativeTransform(RelativeRotationCache.RotatorToQuat(GetRelativeRotation()), GetRelativeLocation(), GetRelativeScale3D());
	return RelativeTransform;
}

// 내 월드 트랜스폼 반환
const FTransform& USceneComponent::GetComponentTransform() const
{
	return ComponentToWorld;
}

void USceneComponent::SetRelativeTransform(const FTransform& NewRelativeTransform)
{
	// 내 로컬 트랜스폼 갱신
	RelativeTransform = NewRelativeTransform;

	UpdateComponentToWorld();
}

void USceneComponent::SetWorldTransform(const FTransform& NewTransform)
{	
	ComponentToWorld = NewTransform;	

	UpdateRelativeTransform();
}

void USceneComponent::Pick(bool bPicked)
{
	bIsPicked = bPicked;
	for (auto& Child : AttachChildren)
	{
		Child->Pick(bPicked);
	}
}

void USceneComponent::SetupAttachment(USceneComponent* InParent, EAttachmentRule AttachmentRule)
{
	if (InParent)
	{
		if (AttachParent != nullptr)
		{
			AttachParent->RemoveChild(this);
		}
		AttachParent = InParent;
		InParent->AttachChildren.Add(this);

		if (AttachmentRule == EAttachmentRule::KeepRelative)
		{
			// RelativeTransform 유지
			UpdateComponentToWorld();
		}
		else if (AttachmentRule == EAttachmentRule::KeepWorld)
		{
			UpdateRelativeTransform();
			// World 유지
		}
		else if (AttachmentRule == EAttachmentRule::SnapToTarget)
		{
			RelativeTransform = FTransform();
			UpdateComponentToWorld();
		}
		
		// ApplyParentWorldTransform(InParent->GetComponentTransform());
	}
	else
	{
		UE_LOG("Parent is nullptr");
	}
}

void USceneComponent::RemoveChild(USceneComponent* Child)
{
	AttachChildren.Remove(Child);
}

void USceneComponent::RemoveAllChildren()
{
	AttachChildren.Empty();
}


// void USceneComponent::ApplyParentWorldTransform()
// {
// 	FMatrix ParentWorld = AttachParent->GetComponentTransform().GetMatrix();
// 	FMatrix MyLocal = RelativeTransform.GetMatrix();
//
// 	FMatrix NewMatrix = MyLocal * ParentWorld.Inverse();
//
// 	// 내 로컬 트랜스폼 갱신
// 	SetRelativeTransform(NewMatrix.GetTransform());
// }


void USceneComponent::UpdateChildTransforms()
{
	for (USceneComponent* ChildComp : AttachChildren)
	{
		if (ChildComp != nullptr)
		{
			ChildComp->UpdateComponentToWorld();
		}
	}
}
// Relative 기반 World Update
void USceneComponent::UpdateComponentToWorld()
{
	if (AttachParent != nullptr)
	{
		const FTransform& ParentToWorld = AttachParent->GetComponentTransform();
		ComponentToWorld = RelativeTransform * ParentToWorld;
	}
	else
	{
		ComponentToWorld = RelativeTransform;
	}

	OnTransformation();

	if (AttachChildren.Num() > 0)
	{
		UpdateChildTransforms();
	}
}
// World 기반 Relative Update
void USceneComponent::UpdateRelativeTransform()
{
	if (AttachParent != nullptr)
	{
		const FTransform& ParentToWorld = AttachParent->GetComponentTransform();
		RelativeTransform = ComponentToWorld * ParentToWorld.Inverse();
	}
	else
	{
		RelativeTransform = ComponentToWorld;
	}

	OnTransformation();

	if (AttachChildren.Num() > 0)
	{
		UpdateChildTransforms();
	}
}

void USceneComponent::OnTransformation()
{
}

void USceneComponent::SetVisibility(bool bNewVisibility)
{
	if (IsA<UPrimitiveComponent>())
	{
		dynamic_cast<UPrimitiveComponent*>(this)->SetDirty(true);
	}
	// UE5 - Visibility 변경시 Flag Update
	bVisible = bNewVisibility;
	const TArray<USceneComponent*>& AttachedChildren = AttachChildren;
	if (AttachedChildren.Num() <= 0)
		return;
	
	TArray<USceneComponent*> ChildrenStack;

	for (auto child : AttachedChildren)
	{
		ChildrenStack.Push(child);
	}

	while (ChildrenStack.Num() > 0)
	{
		USceneComponent* CurrentComp = ChildrenStack.Pop();
		if (CurrentComp != nullptr)
		{
			for (auto child : CurrentComp->AttachChildren)
			{
				ChildrenStack.Push(child);
			}

			CurrentComp->SetVisibility(bNewVisibility);
		}
	}
}
