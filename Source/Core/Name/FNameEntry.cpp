#include "Core/Name/FNameEntry.h"

FNameEntry::FNameEntry(const FString& InName)
: Name(InName)
{
}

const FString& FNameEntry::GetName() const
{
    return Name;
}
