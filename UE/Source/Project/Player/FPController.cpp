
#include "FPController.h"

#include "FPCharacter.h"
#include "Combat/FPCombat.h"
#include "Kismet/GameplayStatics.h"
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

void AFPController::OnStateLoaded(const FFPControllerState aState)
{
	myState = aState;
		
	// Load level
	auto& levelGen = UMainSingelton::GetLevelGenerator();
	levelGen.GenerateLevelOrder(myState.mySeed);
	myState.myInArena ?
		levelGen.LoadArena(myState.myArenaIndex) :
		levelGen.LoadSection(myState.myArenaIndex);

	if (myState.myHasSword)
		if (const auto character = GetFPCharacter())
			if (const auto combat = character->GetCombat())
				combat->SetHasSword(true);
}

FFPControllerState AFPController::GetState() const
{
	FFPControllerState state = myState;
	if (const auto character = GetFPCharacter())
		if (const auto combat = character->GetCombat())
			state.myHasSword = combat->HasSword();
	return state; 
}

void AFPController::CharacterKilled()
{
	if (myState.myArenaIndex)
    {
    	myState.myRespawnCount++;
    	if (myState.myRespawnCount > myRespawns)
    		myState.myArenaIndex = 0;
    }

	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::DEATH);

	const auto character = GetFPCharacter();
	character->DisableInput(this);
	
	LOG("Character killed");
}

void AFPController::Respawn()
{
	LOG("Respawn");
	myState.myInArena ?
		EnterArena() : EnterSection();
}

void AFPController::EnterSection()
{
	myState.myInArena = false;
	SaveState();
	UGameplayStatics::OpenLevel(GetWorld(), "Base");
}

void AFPController::EnterArena()
{
	myState.myInArena = true;
	SaveState();
	UGameplayStatics::OpenLevel(GetWorld(), "Base");
}

bool AFPController::SetCheckpoint(ACheckpoint* aCheckpoint)
{
	CHECK_RETURN_LOG(myCheckpoint == aCheckpoint, "Checkpoint already set", false);
	LOG("Checkpoint set"); 
	myState.myRespawnCount = 0;
	myState.myArenaIndex++;
	myCheckpoint = aCheckpoint;
	myCheckpoint->Activate(this);
	return true;
}

void AFPController::TravelCheckpoint(const ACheckpoint* aCheckpoint)
{
	CHECK_RETURN_LOG(aCheckpoint != myCheckpoint, "Attempted to travel to unknown checkpoint");
	LOG("Travel checkpoint");
	EnterSection();
}

void AFPController::SetEnablePawnControls(const bool aEnabled)
{
	const auto character = GetFPCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	aEnabled ?		
		character->EnableInput(this) :
		character->DisableInput(this);
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
