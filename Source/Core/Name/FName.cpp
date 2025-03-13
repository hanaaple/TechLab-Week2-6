#include "Core/Name/FName.h"

FName::FName()
    : ComparisonIndex(INDEX_NONE), DisplayIndex(INDEX_NONE), Number(0)
{
}

FName::FName(const FString& InName, uint32 InNumber)
: ComparisonIndex(FNameTable::Get().FindOrAddLower(InName)),
  DisplayIndex(FNameTable::Get().FindOrAdd(InName)), // 원본 문자열 저장
  Number(InNumber)
{
    //std::cout<<InName.ToStdString()<<" "<<ComparisonIndex<<" "<<DisplayIndex<<" "<<Number<<std::endl;
}

bool FName::operator==(const FName& Other) const
{
    return FNameTable::Get().GetNameEntry(ComparisonIndex).GetLowerCaseHash()
    == FNameTable::Get().GetNameEntry(Other.ComparisonIndex).GetLowerCaseHash();
    //return ComparisonIndex == Other.ComparisonIndex && Number == Other.Number;
}

bool FName::operator!=(const FName& Other) const
{
    return !(*this == Other);
}
// 대소문자 구별 비교
bool FName::EqualsCaseSensitive(const FName& Other) const
{
    return FNameTable::Get().GetNameEntry(DisplayIndex).GetName() ==
           FNameTable::Get().GetNameEntry(Other.DisplayIndex).GetName();
}
// 정렬용 비교 함수
int32 FName::Compare(const FName& Other) const
{
    if (ComparisonIndex < Other.ComparisonIndex) return -1;
    if (ComparisonIndex > Other.ComparisonIndex) return 1;
    return 0;
}
FString FName::ToString() const
{
    return FNameTable::Get().GetName(DisplayIndex);
}
/*
bool FName::EqualsIgnoreCase(const FName& Other) const
{
    return FNameTable::Get().GetNameEntry(ComparisonIndex).GetLowerCaseHash()
    == FNameTable::Get().GetNameEntry(Other.ComparisonIndex).GetLowerCaseHash();
}*/

