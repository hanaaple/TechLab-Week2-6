#include "Core/Name/FNameTable.h"

int32 FNameTable::FindOrAdd(const FString& Name)
{
    int32* It = NameMap.Find(Name);
    if (It != nullptr)
    {
        return *It;
    }

    int32 NewIndex = static_cast<int32>(NameEntries.Num());
    NameEntries.Emplace(FNameEntry(Name));
    NameMap[Name] = NewIndex;

    return NewIndex;
}

FString FNameTable::GetName(int32 Index) const
{
    if (Index >= 0 && Index < static_cast<int32>(NameEntries.Num()))
    {
        return NameEntries[Index].GetName();
    }
    return "None";
}
