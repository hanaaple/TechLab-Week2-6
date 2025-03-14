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
	virtual FName GetClassFName() const{ return StaticClassFName_Internal(); }
	virtual FName GetParentClassFName() const { return StaticClassFName_Internal(); }
	//virtual const UObject* GetParentClass() const { return nullptr; }
	virtual const UObject* GetParentClass() const { return nullptr; }
	// Name 기반의 IsA 탐색 (기본적으로 FName을 이용한 비교)
	bool IsA(const FName& ClassName) const
	{
		const UObject* Current = this;
		while (Current)
		{
			std::cout << "Checking: " << Current->GetClassFName().ToString().ToStdString() << std::endl;

			if (Current->GetClassFName() == ClassName)
			{
				return true;
			}
			const UObject* Next = Current->GetParentClass();
			if (Next == Current || Next == nullptr)
			{
				break;
			}
			Current = Next;
		}
		return false;
	}
	// Type 기반의 IsA 탐색 (템플릿을 이용한 비교)
	template <typename T>
	bool IsA() const
	{
		return IsA(T::StaticClassFName_Internal());
	}
//
	/*template<typename T>
		requires std::derived_from<T, UObject>
	
	static bool const IsA()
	{
		return IsA(T::StaticClassFName_Internal());
	}
	static bool const IsA(FName ClassName){if (ClassName==GetClassFName())return true;return false;}*/
public:
	uint32 GetUUID() const { return UUID; }
	uint32 GetInternalIndex() const { return InternalIndex; }
};
// 자동 RTTI 매크로 (UObject 하위 클래스 전부 자동 적용)
#define DECLARE_OBJECT(ClassName, ParentClass) \
public: \
using Super = ParentClass; \
static FName StaticClassFName_Internal() { return #ClassName; } \
virtual FName GetClassFName() const override { return StaticClassFName_Internal(); } \
virtual FName GetParentClassFName() const override { return Super::StaticClassFName_Internal(); } \
virtual const UObject* GetParentClass() const override \
{ \
static Super ParentInstance; /* 부모 인스턴스를 생성하여 반환 */ \
return &ParentInstance; \
}
/*
static bool IsA(FName ClassName) \
{ \
if (GetClassFName() == ClassName) \
return true; \
return Super::IsA(ClassName); \
}\
template<typename T> \
requires std::derived_from<T, UObject>\
static bool IsA() { return IsA(T::StaticClassFName_Internal()); }*/