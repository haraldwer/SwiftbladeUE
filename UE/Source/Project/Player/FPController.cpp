
#include "FPController.h"

#include "FPCharacter.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/UI/Menus/MenuManager.h"

void AFPController::BeginPlay()
{
	Super::BeginPlay();
}

void AFPController::SetEnablePawnControls(const bool aEnabled)
{
	if (!myCharacter)
		myCharacter = Cast<AFPCharacter>(GetPawn());
	CHECK_RETURN_LOG(!myCharacter, "Character nullptr");
	aEnabled && myCharacter ?
		AttachToPawn(myCharacter) :
		DetachFromPawn();
}

void AFPController::SetupInputComponent()
{
	Super::SetupInputComponent();

	CHECK_RETURN_LOG(!InputComponent, "Input nullptr");
	InputComponent->BindAction("Pause", IE_Pressed, this, &AFPController::PausePressed);

	LOG("Input setup");
}

void AFPController::PausePressed()
{
	const auto menuManager = UMainSingelton::GetMenuManger();
	CHECK_RETURN(!menuManager);

	if (menuManager->IsAnyMenuOpen())
	{
		if (!menuManager->IsMenuTop(EMenuType::MAIN))
			menuManager->PopMenu();
	}
	else
		menuManager->OpenMenu(EMenuType::PAUSE);
}
