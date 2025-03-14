#include "Core/Name/FNameTable.h"

int32 FNameTable::FindOrAdd(const FString& Name)
{
    //std::cout<<Name.ToStdString()<<" ";
    int32* It = NameMap.Find(Name);
    if (It != nullptr)
    {
    //std::cout<<NameEntries[*It].GetName().ToStdString()<<std::endl;
        return *It;
    }
    int32 NewIndex = static_cast<uint32>(NameEntries.Num());
    NameEntries.Emplace(FNameEntry(Name));
    NameMap[Name] = NewIndex;

    return NewIndex;
}
int32 FNameTable::FindOrAddLower(const FString& Name)
{
    FString LowerName = Name.ToLower();

    int32* It = NameMap.Find(LowerName);
    if (It != nullptr)
    {
        return *It;
    }
    int32 NewIndex = static_cast<int32>(NameEntries.Num());
    NameEntries.Emplace(FNameEntry(LowerName));
    NameMap[LowerName] = NewIndex;

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

const FNameEntry& FNameTable::GetNameEntry(int32 Index) const
{
    return NameEntries[Index];
}