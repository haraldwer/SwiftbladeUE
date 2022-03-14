
#include "FPController.h"

#include "FPCharacter.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/UI/Menus/MenuManager.h"
#include "Project/UI/Prompts/PromptManager.h"

void AFPController::BeginPlay()
{
	Super::BeginPlay();
}

AFPCharacter* AFPController::GetFPCharacter() const
{
	return Cast<AFPCharacter>(GetPawn());
}

void AFPController::CharacterCreated(AFPCharacter* aCharacter)
{
	myStartLocation = aCharacter->GetActorLocation();
}

void AFPController::CharacterKilled()
{
	if (myLatestCheckpoint)
    {
    	myRespawnCount++;
    	if (myRespawnCount > myRespawns)
    		myLatestCheckpoint = nullptr;
    }

	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::DEATH);

	const auto character = GetFPCharacter();
	character->DisableInput(this);
	
	LOG("Character killed");
}

void AFPController::Respawn()
{
	const auto oldCharacter = GetFPCharacter();

	const auto newCharacter = Cast<AFPCharacter>(GetWorld()->SpawnActor(myCharacterBlueprint.Get())); 
	CHECK_RETURN_LOG(!newCharacter, "Character nullptr");
	auto location = 
		newCharacter->SetActorLocation(
			myLatestCheckpoint ?
				myLatestCheckpoint->GetActorLocation() + myCheckpointOffset : myStartLocation);

	Possess(newCharacter);
	newCharacter->OnRespawned();

	CHECK_RETURN_LOG(!oldCharacter, "Old Character nullptr");
	oldCharacter->Destroy();
	LOG("Old Character destroyed");
}

void AFPController::SetCheckpoint(ACheckpoint* aCheckpoint)
{
	CHECK_RETURN_LOG(myLatestCheckpoint == aCheckpoint, "Checkpoint already set");
	const auto character = GetCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	LOG("Checkpoint set"); 
	myRespawnCount = 0;
	myLatestCheckpoint = aCheckpoint;
	myLatestCheckpoint->OnActivated();
}

void AFPController::SetEnablePawnControls(const bool aEnabled)
{
	const auto character = GetCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	aEnabled && character ?
		AttachToPawn(character) :
		DetachFromPawn();
}

void AFPController::SetupInputComponent()
{
	Super::SetupInputComponent();

	CHECK_RETURN_LOG(!InputComponent, "Input nullptr");
	InputComponent->BindAction("Pause", IE_Pressed, this, &AFPController::PausePressed);
}

void AFPController::PausePressed()
{
	auto& menuManager = UMainSingelton::GetMenuManger();
	if (menuManager.IsAnyMenuOpen())
	{
		if (!menuManager.IsMenuTop(EMenuType::MAIN))
			menuManager.PopMenu();
	}
	else
		menuManager.OpenMenu(EMenuType::PAUSE);
}
