#include "Core/Name/FName.h"

FName::FName()
    : ComparisonIndex(INDEX_NONE), Number(0)
{
}

FName::FName(const FString& InName, uint32 InNumber)
    : ComparisonIndex(FNameTable::Get().FindOrAdd(InName)), Number(InNumber)
{
}

bool FName::operator==(const FName& Other) const
{
    return ComparisonIndex == Other.ComparisonIndex && Number == Other.Number;
}

bool FName::operator!=(const FName& Other) const
{
    return !(*this == Other);
}

FString FName::ToString() const
{
    return FNameTable::Get().GetName(ComparisonIndex);
}
