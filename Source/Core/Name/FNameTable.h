#pragma once

#include "Core/Container/Map.h"
#include "Core/Name/FNameEntry.h"
#include "Core/AbstractClass/Singleton.h"
#include "Core/Container/Array.h"

class FNameTable : public TSingleton<FNameTable>
{
public:
    /**
 * 문자열을 테이블에서 찾거나 새로 추가한다.
 * - 이미 존재하는 경우 해당 문자열의 인덱스를 반환
 * - 존재하지 않는 경우 새로운 FNameEntry를 추가하고 인덱스를 반환
 */
    int32 FindOrAdd(const FString& Name);
    //주어진 인덱스에 해당하는 문자열 반환
    FString GetName(int32 Index) const;

private:
    //FString 값으로 NameEntries의 index 반환
    TMap<FString, int32> NameMap;
    //실질적 데이터 저장
    TArray<FNameEntry> NameEntries;
};
