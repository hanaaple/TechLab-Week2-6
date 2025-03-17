#pragma once
#include "Actor.h"

class ATorus : public AActor
{
    DECLARE_OBJECT(ACone, AActor)
    using Super = AActor;
public:
    ATorus();
    virtual ~ATorus() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;
};
