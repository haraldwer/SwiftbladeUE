#include "WidgetBase.h"

#include "Project/Player/FPController.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/CustomCamera.h"

void UWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidgetBase::AddToCamera()
{
	const int zOrder = GetZOrder();
	if (!myCameraWidget)
	{
		AddToViewport(zOrder);
		return;
	}
	
	const auto controller = UMainSingelton::GetLocalController();
	const auto camera = controller ? controller->GetCamera() : nullptr;
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
	if (!myCameraWidget)
	{
		RemoveFromViewport();
		return;
	}
	
	const auto controller = UMainSingelton::GetLocalController();
	const auto camera = controller ? controller->GetCamera() : nullptr;
	if (!camera)
	{
		RemoveFromViewport();
		LOG("Failed to remove widget to in-game view");
		return;
	}
	camera->RemoveWidget(this);
}
