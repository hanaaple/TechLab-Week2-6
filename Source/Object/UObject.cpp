#include "UObject.h"

UObject::UObject()
{
    //AutoSetName();
}

UObject::~UObject()
{
}
/*void UObject::AutoSetName()
{
    Name = FName(GetClassFName().ToString(), this->GetUUID()); // "클래스명_UUID" 형식으로 저장
    std::cout<<Name<<std::endl;
}*/