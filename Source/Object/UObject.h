#pragma once
#include <memory>
#include "Core/HAL/PlatformType.h"
#include "Core/Name/FName.h"


// TODO: RTTI 구현하면 enable_shared_from_this 제거
class UObject/* : public std::enable_shared_from_this<UObject>*/
{
	//DECLARE_OBJECT(UObject, UObject)
	friend class FObjectFactory;

	uint32 UUID = 0;
	uint32 InternalIndex; // Index of GUObjectArray
protected:
	static FName StaticClassFName_Internal() { return "UObject"; }
public:
	UObject();
	virtual ~UObject();
public:
	static FName GetClassFName() { return StaticClassFName_Internal(); }
	static FName GetParentClassFName() { return StaticClassFName_Internal(); }
	//virtual const UObject* GetParentClass() const { return nullptr; }

//
	template<typename T>
		requires std::derived_from<T, UObject>
	static bool IsA()
	{
		return IsAByName(T::GetClassFName());
	}
	static bool IsA(FName ClassName){if (ClassName==GetClassFName())return true;return false;}
public:
	uint32 GetUUID() const { return UUID; }
	uint32 GetInternalIndex() const { return InternalIndex; }
};
// 자동 RTTI 매크로 (UObject 하위 클래스 전부 자동 적용)
#define DECLARE_OBJECT(ClassName, ParentClass) \
public: \
using Super = ParentClass; \
static FName StaticClassFName_Internal() { return #ClassName; } \
static FName GetClassFName() { return StaticClassFName_Internal(); } \
static FName GetParentClassFName() { return Super::StaticClassFName_Internal(); } \
static bool IsA(FName ClassName) \
{ \
if (GetClassFName() == ClassName) \
return true; \
return Super::IsA(ClassName); \
}\
template<typename T> \
requires std::derived_from<T, UObject>\
static bool IsA() { return IsA(T::StaticClassFName_Internal()); }