#include "Core/Name/FNameEntry.h"

FNameEntry::FNameEntry(const FString& InName)
: Name(InName), HashLowerCase(ComputeLowerCaseHash(InName))
{
}

const FString& FNameEntry::GetName() const
{
    return Name;
}
uint32 FNameEntry::GetLowerCaseHash() const
{
    return HashLowerCase;
}