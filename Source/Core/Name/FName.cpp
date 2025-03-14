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
}
FName::FName(const char* pStr, uint32 InNumber)
    : FName(FString(pStr), InNumber) // FString 변환 후 기존 생성자 호출
{
}
bool FName::operator==(const FName& Other) const
{
    return ComparisonIndex == Other.ComparisonIndex&&Number == Other.Number;
    //return ComparisonIndex == Other.ComparisonIndex && Number == Other.Number;
}

bool FName::operator!=(const FName& Other) const
{
    return !(*this == Other);
}
// 대소문자 구별 비교
bool FName::EqualsCaseSensitive(const FName& Other) const
{
    return DisplayIndex == Other.DisplayIndex&&Number==Other.Number;
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
const FString& FName::ToStringRef() const
{
    return FNameTable::Get().GetName(DisplayIndex); // ✅ 참조 반환
}
int32 FName::GetNumber() const
{
    return Number;
}
bool FName::IsValid() const
{
    return ComparisonIndex != INDEX_NONE && DisplayIndex != INDEX_NONE;
}
void FName::SetIndex(const FName& Other)
{
    ComparisonIndex = Other.ComparisonIndex;
    DisplayIndex = Other.DisplayIndex;
}

/*
bool FName::EqualsIgnoreCase(const FName& Other) const
{
    return FNameTable::Get().GetNameEntry(ComparisonIndex).GetLowerCaseHash()
    == FNameTable::Get().GetNameEntry(Other.ComparisonIndex).GetLowerCaseHash();
}*/

