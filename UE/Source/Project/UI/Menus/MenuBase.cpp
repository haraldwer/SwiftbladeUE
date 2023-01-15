#include "MenuBase.h"

UMenuBase::UMenuBase() : Super()
{
	myCameraWidget = false;
}

void UMenuBase::Init(const EMenuType aMenuType)
{
	myMenuType = aMenuType;
}


