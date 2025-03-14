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

FTransform USceneComponent::GetRelativeTransform() const
{
	//const FTransform RelativeTransform(RelativeRotationCache.RotatorToQuat(GetRelativeRotation()), GetRelativeLocation(), GetRelativeScale3D());
	return RelativeTransform;
}

// 내 월드 트랜스폼 반환
const FTransform& USceneComponent::GetComponentTransform() const
{
	if (AttachParent)
	{
		// 부모가 있을 경우 부모 월드 * 내 로컬
		FMatrix ParentWorld = AttachParent->GetComponentTransform().GetMatrix();
		FMatrix MyLocal = RelativeTransform.GetMatrix();

		FMatrix NewMatrix = MyLocal * ParentWorld;
		return NewMatrix.GetTransform();
	}

	return RelativeTransform;
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
	
	// -> ComponentToWorld = CalcNewComponentToWorld(RelativeTransform, Parent, SocketName); ????
	// -> Relative -> Parent와 상대적 Transform을 구해서 넣어줘야됨.		NewTransform.GetRelativeTransform(ParentToWorld);
	
	
	if (AttachParent != nullptr)
	{
		// Parent 기반 RelativeTransform Update
		
		const FTransform& ParentToWorld = AttachParent->GetComponentTransform();
		// Parent World 기준 A의 RelativeTransform 새로 갱신?
		RelativeTransform = NewTransform.GetRelativeTransform(ParentToWorld);
		// 부모가 있다면, RelativeTransform = 부모의 Inverse(ComponentToWorld) * NewTransform 계산.
	}
	else
	{
		RelativeTransform = NewTransform;
	}

	if (AttachChildren.Num() > 0)
	{
		UpdateChildTransforms();
	}
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
		AttachParent = InParent;
		InParent->AttachChildren.Add(this);

		if (AttachmentRule == EAttachmentRule::KeepRelative)
		{
			// RelativeTransform 유지
		}
		else if (AttachmentRule == EAttachmentRule::KeepWorld)
		{
			// World 유지
		}
		else if (AttachmentRule == EAttachmentRule::SnapToTarget)
		{
			// RelativeTransform = FTransform()
		}
		
		// ApplyParentWorldTransform(InParent->GetComponentTransform());
	}
	else
	{
		UE_LOG("Parent is nullptr");
	}
}

void USceneComponent::ApplyParentWorldTransform()
{
	FMatrix ParentWorld = AttachParent->GetComponentTransform().GetMatrix();
	FMatrix MyLocal = RelativeTransform.GetMatrix();

	FMatrix NewMatrix = MyLocal * ParentWorld.Inverse();

	// 내 로컬 트랜스폼 갱신
	SetRelativeTransform(NewMatrix.GetTransform());
}


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

void USceneComponent::UpdateComponentToWorld()
{
	if (AttachParent != nullptr)
	{
		ComponentToWorld = .GetRelativeTransform(ParentToWorld); 
	}
	else
	{
		ComponentToWorld = RelativeTransform;
	}

	if (AttachChildren.Num() > 0)
	{
		UpdateChildTransforms();
	}
	
	if (AttachChildren.Num() > 0)
	{
		UpdateChildTransforms();
	}
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