﻿#pragma once

#include "Core/Container/String.h"
#include "Core/Name/FNameTable.h"

class FName
{
public:
    FName();
    FName(const FString& InName, uint32 InNumber = 0);
    FName(const char* pStr, uint32 InNumber = 0); // char* 버전 추가

    //대소문자 무시 비교(소문자 해시값으로 비교)
    bool operator==(const FName& Other) const;
    bool operator!=(const FName& Other) const;
    // ✅ std::cout 지원을 위한 연산자 오버로딩 추가
    friend std::ostream& operator<<(std::ostream& os, const FName& Name)
    {
        return os << Name.ToString().ToStdString();
    }

    // 정렬용 비교 함수
    int32 Compare(const FName& Other) const;
    //대소문자 고려 비교(해시값으로 비교).. 하지만 FMap에서 비교 시에 == 연산자를 사용해서 사용 불가
    bool EqualsCaseSensitive(const FName& Other) const;

    //bool EqualsIgnoreCase(const FName& Other) const;
    //@return FString 형태의 문자열
    FString ToString() const;
    std::string ToStdString() const;
    const char* ToCharString() const;
    const FString& ToStringRef() const;
    int32 GetNumber() const;
    bool IsValid() const;
    void SetIndex(const FName& Other);
    int32 GetDisplayIndex() const;
    int32 Length() const { return ToString().Len(); }

private:
    //네임 테이블에서 이 FName이 저장된 위치의 인덱스(소문자로 변경된 해시)
    int32 ComparisonIndex;
    //원본 문자열을 가져오기 위한 인덱스
    int32 DisplayIndex;

    //동일한 이름이 여러 개 존재할 경우 이를 구분하기 위한 숫자
    uint32 Number;

    //디버깅용. 실사용X
#ifdef _DEBUG
    FString DebugString;
#endif
};
