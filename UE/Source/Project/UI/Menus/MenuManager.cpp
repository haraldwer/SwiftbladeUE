#include "MenuManager.h"

#include "MenuBase.h"
#include "Project/Player/FPController.h"
#include "Project/Utility/MainSingelton.h"

AMenuManager::AMenuManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMenuManager::ToggleMenu(const EMenuType aMenu)
{
	if (IsMenuOpen(aMenu))
		CloseMenu(aMenu);
	else
		OpenMenu(aMenu);
}

void AMenuManager::OpenMenu(const EMenuType aMenu)
{
	CHECK_RETURN_LOG(IsMenuOpen(aMenu), "Menu already open");
	const auto menuBase = GetMenuInstance(aMenu);
	CHECK_RETURN_LOG(!menuBase, "Menu instance nullptr");
	myMenuStack.Add(menuBase);
	menuBase->AddToScreen(menuBase->GetZOrder());
	SetInputMode(menuBase->GetInputMode());
	UpdateExclusiveVisibility();
	menuBase->OnOpen();
}

void AMenuManager::CloseMenu(const EMenuType aMenu)
{
	const auto menu = GetMenu(aMenu);
	CHECK_RETURN_LOG(!menu, "Menu already closed");
	menu->OnClose();
	menu->RemoveFromScreen();
	myMenuStack.Remove(menu);
	if (myMenuStack.Num() > 0)
		SetInputMode(myMenuStack.Top()->GetInputMode());
	else ResetInputMode();
	UpdateExclusiveVisibility();
}

void AMenuManager::PopMenu()
{
	if (myMenuStack.Num() <= 0)
		return;
	CloseMenu(myMenuStack.Last()->GetMenuType());
}

bool AMenuManager::IsMenuOpen(const EMenuType aMenu) const
{
	return GetMenuConst(aMenu) != nullptr;
}

bool AMenuManager::IsMenuTop(const EMenuType aMenu) const
{
	const auto top = myMenuStack.Top();
	return top && top->GetMenuType() == aMenu;
}

bool AMenuManager::IsAnyMenuOpen() const
{
	return myMenuStack.Num() > 0;
}

UMenuBase* AMenuManager::GetMenu(const EMenuType aMenu)
{
	for (const auto& it : myMenuStack)
		if (it->GetMenuType() == aMenu)
			return it;
	return nullptr;
}

const UMenuBase* AMenuManager::GetMenuConst(const EMenuType aMenu) const
{
	for (const auto& it : myMenuStack)
		if (it->GetMenuType() == aMenu)
			return it;
	return nullptr;
}

void AMenuManager::BeginPlay()
{
	Super::BeginPlay();
	
}

UMenuBase* AMenuManager::GetMenuInstance(const EMenuType aMenu)
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
	CHECK_RETURN_LOG(!menuBase, "Menu does not inherit from SMenuBase", nullptr);
	myMenus.Add(menuBase);
	menuBase->Init(aMenu);
	return menuBase;
}

void AMenuManager::UpdateExclusiveVisibility()
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

void AMenuManager::SetInputMode(const FMenuInputMode& aInputMode)
{
	const auto controller = UMainSingelton::GetLocalController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr when setting input mode");
	FInputModeGameAndUI mode;
	mode.SetHideCursorDuringCapture(aInputMode.myHideCursor);
	mode.SetLockMouseToViewportBehavior(aInputMode.myLockMouseToViewport ? EMouseLockMode::LockAlways : EMouseLockMode::DoNotLock);
	controller->SetInputMode(mode);
	controller->SetShowMouseCursor(!aInputMode.myHideCursor);
	controller->SetEnablePawnControls(aInputMode.myAllowPlayerControl);
}

void AMenuManager::ResetInputMode()
{
	const auto controller = UMainSingelton::GetLocalController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr when setting input mode");
	controller->SetInputMode(FInputModeGameOnly());
	controller->SetShowMouseCursor(false);
	controller->SetEnablePawnControls(true);
}
