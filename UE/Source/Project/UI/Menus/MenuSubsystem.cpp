#include "MenuSubsystem.h"

#include "MenuBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/GameInstance.h"
#include "Project/Player/FPController.h"
#include "Project/Utility/GameUtility.h"

UMenuSubsystem& UMenuSubsystem::Get()
{
	const auto subsystem = UGameUtility::GetCurrentWorld().GetSubsystem<UMenuSubsystem>();
	CHECK_ASSERT(!subsystem, "Subsystem nullptr");
	return *subsystem;
}

void UMenuSubsystem::ToggleMenu(const EMenuType aMenu)
{
	if (IsMenuOpen(aMenu))
		CloseMenu(aMenu);
	else
		OpenMenu(aMenu);
}

void UMenuSubsystem::OpenMenu(const EMenuType aMenu)
{
	CHECK_RETURN_LOG(IsMenuOpen(aMenu), "Menu already open");
	const auto menu = GetMenuInstance(aMenu);
	CHECK_RETURN_LOG(!menu, "Menu instance nullptr");
	myMenuStack.Add(menu);
	menu->AddToCamera();
	menu->SetIsFocusable(true); 
	menu->SetFocus();
	menu->SetKeyboardFocus();
	SetInputMode(menu->GetInputMode());
	UpdateExclusiveVisibility();
	menu->OnOpen();
}

void UMenuSubsystem::CloseMenu(const EMenuType aMenu)
{
	const auto menu = GetMenu(aMenu);
	CHECK_RETURN_LOG(!menu, "Menu already closed");
	menu->OnClose();
	menu->SetVisibility(ESlateVisibility::Collapsed);
	menu->RemoveFromCamera();
	menu->SetIsFocusable(false); 
	myMenuStack.Remove(menu);
	if (myMenuStack.Num() > 0)
	{
		const auto top = myMenuStack.Top();
		top->SetFocus();
		top->SetKeyboardFocus(); 
		SetInputMode(top->GetInputMode());
	}
	else ResetInputMode();
	UpdateExclusiveVisibility();
}

void UMenuSubsystem::PopMenu()
{
	if (myMenuStack.Num() <= 0)
		return;
	CloseMenu(myMenuStack.Last()->GetMenuType());
}

void UMenuSubsystem::CloseAll()
{
	while (IsAnyMenuOpen())
		PopMenu();
}

bool UMenuSubsystem::IsMenuOpen(const EMenuType aMenu) const
{
	return GetMenuConst(aMenu) != nullptr;
}

bool UMenuSubsystem::IsMenuTop(const EMenuType aMenu) const
{
	const auto top = myMenuStack.Top();
	return top && top->GetMenuType() == aMenu;
}

bool UMenuSubsystem::IsAnyMenuOpen() const
{
	return myMenuStack.Num() > 0;
}

UMenuBase* UMenuSubsystem::GetMenu(const EMenuType aMenu)
{
	for (const auto& it : myMenuStack)
		if (it->GetMenuType() == aMenu)
			return it;
	return nullptr;
}

const UMenuBase* UMenuSubsystem::GetMenuConst(const EMenuType aMenu) const
{
	for (const auto& it : myMenuStack)
		if (it->GetMenuType() == aMenu)
			return it;
	return nullptr;
}

UMenuBase* UMenuSubsystem::GetMenuInstance(const EMenuType aMenu)
{
	// Try to get menu instance
	for (const auto& menu : myMenus)
		if (menu->GetMenuType() == aMenu)
			return menu;

	// Otherwise create
	const auto blueprint = myMenuBlueprints.Find(aMenu);
	CHECK_RETURN_LOG(!blueprint, "Unable to find menu blueprint", nullptr);
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr", nullptr);
	const auto menu = CreateWidget(world, blueprint->Get());
	CHECK_RETURN_LOG(!menu, "Failed to create menu", nullptr);
	const auto menuBase = Cast<UMenuBase>(menu);
	CHECK_RETURN_LOG(!menuBase, "Menu does not inherit from UMenuBase", nullptr);
	myMenus.Add(menuBase);
	menuBase->Init(aMenu);
	return menuBase;
}

void UMenuSubsystem::UpdateExclusiveVisibility()
{
	CHECK_RETURN(myMenuStack.Num() < 1);
	bool topExclusive = false;
	for (int i = myMenuStack.Num() - 1; i >= 0; i--)
	{
		if (!myMenuStack[i])
			continue;
		myMenuStack[i]->SetVisibility(topExclusive ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		topExclusive = topExclusive || myMenuStack[i]->GetIsExclusive();
	}
}

void UMenuSubsystem::SetInputMode(const FMenuInputMode& aInputMode)
{
	const auto controller = UGameUtility::GetLocalController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr when setting input mode");
	FInputModeGameAndUI mode;
	mode.SetHideCursorDuringCapture(aInputMode.myHideCursor);
	mode.SetLockMouseToViewportBehavior(aInputMode.myLockMouseToViewport ? EMouseLockMode::LockAlways : EMouseLockMode::DoNotLock);
	controller->SetInputMode(mode);
	controller->SetShowMouseCursor(!aInputMode.myHideCursor);
	controller->SetEnablePawnControls(aInputMode.myAllowPlayerControl);
	LOG("Set input mode");
}

void UMenuSubsystem::ResetInputMode()
{
	const auto controller = UGameUtility::GetLocalController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr when setting input mode");
	controller->SetInputMode(FInputModeGameOnly());
	controller->SetShowMouseCursor(false);
	controller->SetEnablePawnControls(true);

	//FSlateApplication::Get().ClearKeyboardFocus();
	//FSlateApplication::Get().ClearAllUserFocus();
	LOG("Reset input mode");
}
