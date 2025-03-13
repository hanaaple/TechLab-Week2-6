#pragma once
#include <memory>
#include "Core/HAL/PlatformType.h"
#include "Core/Name/FName.h"


// TODO: RTTI 구현하면 enable_shared_from_this 제거
class UObject/* : public std::enable_shared_from_this<UObject>*/
{
	friend class FObjectFactory;

	uint32 UUID = 0;
	uint32 InternalIndex; // Index of GUObjectArray
protected:
	static FName StaticClassFName_Internal() { return "UObject"; }
public:
	UObject();
	virtual ~UObject();
public:
	virtual FName GetClassFName() const { return StaticClassFName_Internal(); }
	bool IsA(FName ClassName) const { return GetClassFName() == ClassName; }
	bool IsA(const UObject* Other) const { return Other && GetClassFName() == Other->GetClassFName(); }
	
public:
	uint32 GetUUID() const { return UUID; }
	uint32 GetInternalIndex() const { return InternalIndex; }
};
// 자동 RTTI 매크로 (UObject 하위 클래스 전부 자동 적용)
#define DECLARE_OBJECT(ClassName, ParentClass) \
public: \
using Super = ParentClass; \
static FName StaticClassFName_Internal() { return #ClassName; } \
virtual FName GetClassFName() const override { return StaticClassFName_Internal(); }
