﻿#include "Actor.h"
#include "Debug/DebugConsole.h"
#include "Object/World/World.h"
#include "Static/FEditorManager.h"

void AActor::BeginPlay()
{
	for (auto& Component : Components)
	{
		Component->BeginPlay();
	}
}

void AActor::Tick(float DeltaTime)
{
	for (auto& Component : Components)
	{
		if (Component->CanEverTick())
		{
			Component->Tick(DeltaTime);
		}
	}
}

void AActor::LateTick(float DeltaTime)
{
}

void AActor::Destroyed()
{
	EndPlay(EEndPlayReason::Destroyed);
}


void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<UActorComponent*> DestroyComponents;
	if (RootComponent == nullptr)
		return;
	DestroyComponents.Add(RootComponent);
	
	while (DestroyComponents.Num() > 0)
	{
		UActorComponent* Component = DestroyComponents[0];
		DestroyComponents.Remove(Component);
		if (Component->GetOwner() != this)
		{
			if (Component->IsA<USceneComponent>())
			{
				USceneComponent* SceneComponent = dynamic_cast<USceneComponent*>(Component);
				SceneComponent->SetupAttachment(nullptr);
			}
			continue;
		}
				
		if (Component->IsA<USceneComponent>())
		{
			USceneComponent* SceneComponent = dynamic_cast<USceneComponent*>(Component);
			for (auto* Child : SceneComponent->GetAttachChildren())
			{
				DestroyComponents.Add(Child);
			}
			SceneComponent->RemoveAllChildren();
		}

		Component->EndPlay(EndPlayReason);
		RemoveComponent(Component);
	}
}

void AActor::ActivateComponent()
{
	while (ToActiveComponents.Num() > 0)
	{
		UActorComponent* Component = ToActiveComponents[0];
		ToActiveComponents.Remove(Component);	
		Component->Activate();
	}
}

void AActor::Pick()
{
	if (RootComponent)
	{
		RootComponent->Pick(true);
	}
}

void AActor::UnPick()
{
	if (RootComponent)
	{
		RootComponent->Pick(false);
	}	
}

FTransform AActor::GetActorTransform() const
{
	return RootComponent != nullptr ? RootComponent->GetComponentTransform() : FTransform();
}

void AActor::SetActorTransform(const FTransform& NewTransform)
{
	if (RootComponent)
	{
		RootComponent->SetWorldTransform(NewTransform);
	}
	else
	{
		UE_LOG("RootComponent is nullptr");
	}
}

void AActor::SetActorRelativeTransform(const FTransform& NewRelativeTransform)
{
	if (RootComponent)
	{
		RootComponent->SetRelativeTransform(NewRelativeTransform);
	}
	else
	{
		UE_LOG("RootComponent is nullptr");
	}
}

const char* AActor::GetTypeName()
{
	return "Actor";
}

bool AActor::Destroy()
{
	return GetWorld()->DestroyActor(this);
}

void AActor::SetColor(FVector4 InColor)
{
	if (RootComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetCustomColor(InColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetCustomColor(InColor);
		}
	}
}

void AActor::SetUseVertexColor(bool bUseVertexColor)
{
	if (RootComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetUseVertexColor(bUseVertexColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetUseVertexColor(bUseVertexColor);
		}
	}
}
