#pragma once

#include <algorithm>

#include "Core/Container/String.h"

class FNameEntry
{
public:
    explicit FNameEntry(const FString& InName);
    const FString& GetName() const;
    uint32 GetLowerCaseHash() const;
private:
    FString Name;
    uint32 HashLowerCase;  // 소문자로 변환된 해시값 저장
    static uint32 ComputeLowerCaseHash(const FString& Str)
    {
#if IS_WIDECHAR
        std::wstring LowerStr = Str.ToStdString();
        std::transform(LowerStr.begin(), LowerStr.end(), LowerStr.begin(), ::towlower);
        return std::hash<std::wstring>()(LowerStr);
#else
        std::string LowerStr = Str.ToStdString();
        std::transform(LowerStr.begin(), LowerStr.end(), LowerStr.begin(), ::tolower);
        return std::hash<std::string>()(LowerStr);
#endif
    }
};
