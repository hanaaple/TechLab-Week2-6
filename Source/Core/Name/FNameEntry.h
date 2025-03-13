#pragma once

#include "Core/Container/String.h"

class FNameEntry
{
public:
    explicit FNameEntry(const FString& InName);
    const FString& GetName() const;

private:
    /**
     * 현재는 FString만 저장하지만 문자열 길이 등의 정보 추가 저장 가능
     */
    FString Name;
};
