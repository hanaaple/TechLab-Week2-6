#include "USceneComponent.h"
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

// 내 월드 트랜스폼 반환
const FTransform USceneComponent::GetWorldTransform()
{
	if (AttachParent)
	{
		// 부모가 있을 경우 부모 월드 * 내 로컬
		FMatrix ParentWorld = AttachParent->GetWorldTransform().GetMatrix();
		FMatrix MyLocal = RelativeTransform.GetMatrix();

		FMatrix NewMatrix = MyLocal * ParentWorld;
		return NewMatrix.GetTransform();
	}

	return RelativeTransform;
}

void USceneComponent::SetRelativeTransform(const FTransform& InTransform)
{
	// 내 로컬 트랜스폼 갱신
	RelativeTransform = InTransform;
	FVector Rot = RelativeTransform.GetRotation().GetEuler();

}

void USceneComponent::Pick(bool bPicked)
{
	bIsPicked = bPicked;
	for (auto& Child : AttachChildren)
	{
		Child->Pick(bPicked);
	}
}

void USceneComponent::SetupAttachment(USceneComponent* InParent, bool bUpdateChildTransform)
{
	if (InParent)
	{
		AttachParent = InParent;
		InParent->AttachChildren.Add(this);
		ApplyParentWorldTransform(InParent->GetWorldTransform());
	}
	else
	{
		UE_LOG("Parent is nullptr");
	}
}

void USceneComponent::ApplyParentWorldTransform(const FTransform& ParentWorldTransform)
{
	FMatrix ParentWorld = ParentWorldTransform.GetMatrix();
	FMatrix MyLocal = RelativeTransform.GetMatrix();

	FMatrix NewMatrix = MyLocal * ParentWorld.Inverse();

	// 내 로컬 트랜스폼 갱신
	SetRelativeTransform(NewMatrix.GetTransform());
}

void USceneComponent::SetVisibility(bool bNewVisibility) const
{
	// UE5 - Visibility 변경시 Flag Update

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
			for (auto child : CurrentComp->GetAttachChildren())
			{
				ChildrenStack.Push(child);
			}

			CurrentComp->SetVisibility(bNewVisibility);
			
		}
	}
}
