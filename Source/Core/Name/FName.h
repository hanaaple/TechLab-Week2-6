#pragma once

#include "Core/Container/String.h"
#include "Core/Name/FNameEntry.h"
#include "Core/Name/FNameTable.h"

class FName
{
public:
    FName();
    FName(const FString& InName, uint32 InNumber = 0);

    bool operator==(const FName& Other) const;
    bool operator!=(const FName& Other) const;

    //@return FString 형태의 문자열
    FString ToString() const;

private:
    //네임 테이블에서 이 FName이 저장된 위치의 인덱스
    int32 ComparisonIndex;
    
    //동일한 이름이 여러 개 존재할 경우 이를 구분하기 위한 숫자
    uint32 Number;
};
