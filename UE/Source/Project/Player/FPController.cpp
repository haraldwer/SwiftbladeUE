
#include "FPController.h"

#include "FPCharacter.h"
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
	if (myHasCheckpoint)
    {
    	myRespawnCount++;
    	if (myRespawnCount > myRespawns)
    		myHasCheckpoint = false;
    }

	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::DEATH);

	DetachFromPawn();
	
	const auto character = GetFPCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	character->Destroy();

	LOG("Character killed");
}

void AFPController::Respawn()
{
	CHECK_RETURN_LOG(GetFPCharacter(), "Character already exists, cannot respawn");
	
	// Create character actor
	AActor* actor = GetWorld()->SpawnActor(myCharacterBlueprint.Get());
	CHECK_RETURN_LOG(!actor, "Actor nullptr");
	AFPCharacter* character = Cast<AFPCharacter>(actor);
	CHECK_RETURN_LOG(!character, "Character nullptr");
	auto location = 
		character->SetActorLocation(
			myHasCheckpoint ?
				myCheckpointLocation : myStartLocation);
	AttachToPawn(character);
}

void AFPController::SetCheckpoint(AActor* aActor)
{
	const auto character = GetCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	LOG("Checkpoint set"); 
	myHasCheckpoint = true;
	myRespawnCount = 0;
	myCheckpointLocation = character->GetActorLocation();
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
