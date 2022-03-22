
#include "FPController.h"

#include "FPCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
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
	LOG("Pos: " + FString::SanitizeFloat(myStartLocation.X) + " " + FString::SanitizeFloat(myStartLocation.Y) + " " + FString::SanitizeFloat(myStartLocation.Z));
	EnterSection();
}

void AFPController::CharacterKilled()
{
	if (myArenaIndex)
    {
    	myRespawnCount++;
    	if (myRespawnCount > myRespawns)
    		myArenaIndex = 0;
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
	Possess(newCharacter);
	CHECK_RETURN_LOG(GetPawn() != newCharacter, "Failed to possess new character");
	newCharacter->OnRespawned();
	CHECK_RETURN_LOG(!oldCharacter, "Old Character nullptr");
	oldCharacter->Destroy();
	LOG("Old Character destroyed");
	
	EnterSection();
}

void AFPController::EnterSection() const
{
	auto& levelGen = UMainSingelton::GetLevelGenerator();
	levelGen.LoadSection(myArenaIndex);
	if (const auto character = GetFPCharacter())
		character->SetActorLocation(myStartLocation);
}

void AFPController::EnterArena() const
{
	auto& levelGen = UMainSingelton::GetLevelGenerator();
	levelGen.LoadArena(myArenaIndex);
	if (const auto character = GetFPCharacter())
	{
		character->SetActorLocation(myStartLocation);
		if (const auto movement = character->GetMovementComponent())
			movement->Velocity = FVector();
		character->OnEnterArena();
	}
}

bool AFPController::SetCheckpoint(ACheckpoint* aCheckpoint)
{
	CHECK_RETURN_LOG(myCheckpoint == aCheckpoint, "Checkpoint already set", false);
	LOG("Checkpoint set"); 
	myRespawnCount = 0;
	myCheckpoint = aCheckpoint;
	myCheckpoint->OnActivated();
	myArenaIndex++;
	auto& levelGen = UMainSingelton::GetLevelGenerator();
	levelGen.LoadSection(myArenaIndex);
	return true;
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
