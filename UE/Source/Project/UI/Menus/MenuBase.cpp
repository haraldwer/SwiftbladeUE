#include "MenuBase.h"
#include "MenuManager.h"

void UMenuBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMenuBase::Init(const EMenuType aMenuType)
{
	myMenuType = aMenuType;
}


