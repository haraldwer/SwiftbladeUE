#include "WidgetBase.h"

#include "Project/Player/FPCharacter.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/CustomCamera.h"

void UWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidgetBase::AddToCamera()
{
	const auto character = UMainSingelton::GetLocalPlayer();
	const auto camera = character ? character->GetCamera() : nullptr;
	const int zOrder = GetZOrder();
	if (!camera)
	{
		AddToViewport(zOrder);
		LOG("Failed to add widget to in-game view");
		return;
	}
	camera->AddWidget(this, zOrder);
}

void UWidgetBase::RemoveFromCamera()
{
	const auto character = UMainSingelton::GetLocalPlayer();
	const auto camera = character ? character->GetCamera() : nullptr;
	if (!camera)
	{
		RemoveFromViewport();
		LOG("Failed to remove widget to in-game view");
		return;
	}
	camera->RemoveWidget(this);
}
