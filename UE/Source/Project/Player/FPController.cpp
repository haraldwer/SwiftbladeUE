
#include "FPController.h"

#include "FPCharacter.h"
#include "FPTime.h"
#include "GameDatabase.h"
#include "Components/Leaderboard.h"
#include "Camera/CameraActor.h"
#include "Combat/FPCombat.h"
#include "Components/Blob.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/Gameplay/GameEnd.h"
#include "Project/LevelGen/Level/LevelManager.h"
#include "Project/UI/Menus/MenuManager.h"
#include "Project/UI/Prompts/PromptManager.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/CustomCamera.h"

void AFPController::BeginPlay()
{
	Super::BeginPlay();
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::LEVEL_FADEIN);
}

AFPCharacter* AFPController::GetFPCharacter() const
{
	return Cast<AFPCharacter>(GetPawn());
}

UCustomCamera* AFPController::GetCamera() const
{
	if (const auto character = GetFPCharacter())
		if (const auto camera = character->GetCamera())
			return camera;
	if (const auto camera = GetAutoActivateCameraForPlayer())
		if (const auto cameraComponent = Cast<UCustomCamera>(
			camera->GetComponentByClass(UCustomCamera::StaticClass())))
			return cameraComponent;
	return nullptr;
}

void AFPController::OnStateLoaded(const FFPControllerState aState)
{
	myState = aState;
	if (!myState.mySeed)
		myState.mySeed = FMath::Rand();
	
	// Load level
	auto& levelGen = UMainSingelton::GetLevelManager();
	levelGen.GenerateLevelOrder(myState.mySeed + myState.myArenaIndex);
	myState.myInArena ?
		levelGen.LoadArena(myState.myArenaIndex) :
		levelGen.LoadSection(myState.myArenaIndex);

	if (const auto character = GetFPCharacter())
		character->OnStateLoaded(aState);
}

FFPControllerState AFPController::GetState() const
{
	FFPControllerState state = myState;
	if (const auto character = GetFPCharacter())
	{
		if (const auto combat = character->GetCombat())
			state.myHasSword = combat->HasSword();
		if (const auto time = character->GetTime())
			state.myTime = time->GetScoreTime();
	} 
	return state; 
}

void AFPController::CharacterKilled()
{
	myState.myRespawnCount++;
	if (const auto character = GetFPCharacter())
		character->OnLivesChanged(GetRemainingLives());
	
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::DEATH);

	const auto character = GetFPCharacter();
	character->DisableInput(this);

	LOG("Character killed");

	FLeaderboardSubmission submission;
	submission.mySeed = myState.mySeed;
	submission.myScore = FMath::Rand();
	const auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.Write(submission);

	auto& blob = UMainSingelton::GetGameDB().GetBlob();
	auto blobData = blob.Get();
	blobData.mySeedData.mySeed = myState.mySeed;
	blob.Set(blobData);
}

void AFPController::Respawn()
{
	LOG("Respawn");
	StartTravel(EFPTravelReason::RESPAWN);
}

bool AFPController::TrySetCheckpoint(ACheckpoint* aCheckpoint)
{
	CHECK_RETURN_LOG(!aCheckpoint->IsEnabled(), "Checkpoint not yet enabled", false);
	CHECK_RETURN_LOG(myCheckpoint == aCheckpoint, "Checkpoint already set", false);
	LOG("Checkpoint set"); 
	myState.myRespawnCount = 0;
	myState.myArenaIndex++;
	if (const auto character = GetFPCharacter())
		character->OnLivesChanged(GetRemainingLives());
	myCheckpoint = aCheckpoint;
	myCheckpoint->Activate(this);
	return true;
}

void AFPController::UseCheckpoint(const ACheckpoint* aCheckpoint)
{
	CHECK_RETURN_LOG(aCheckpoint != myCheckpoint, "Attempted to travel to unknown checkpoint");
	StartTravel(EFPTravelReason::CHECKPOINT);
}

void AFPController::StartTravel(const EFPTravelReason aReason)
{
	LOG("Start travel ");
	myTravelReason = aReason;
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::LEVEL_FADEOUT);
}

void AFPController::FinishTravel()
{
	switch (myTravelReason)
	{	
	case EFPTravelReason::RESPAWN:
		{
			if (myState.myRespawnCount >= myRespawns)
			{
				myState.myArenaIndex = 0;
				myState.myRespawnCount = 0;
				myState.mySeed = FMath::Rand();
			}
			myState.myInArena ?
				EnterArena() : EnterSection();
		}
		break;
	case EFPTravelReason::CHECKPOINT:
		EnterSection();
		break;
	case EFPTravelReason::ARENA:
		EnterArena();
		break;
	
	case EFPTravelReason::NONE:
	case EFPTravelReason::COUNT:
	default:
		LOG("Unknown travel reason");
	}
}

void AFPController::ReachEnd(AGameEnd* aGameEnd)
{
	CHECK_RETURN_LOG(!aGameEnd, "GameEnd actor nullptr")
	CHECK_RETURN(myHasReachedEnd);
	
	const auto character = GetFPCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	
	const auto time = character->GetTime();
	const auto scoreTime = time->GetScoreTime();
	//CHECK_RETURN(scoreTime < myMinAllowedTime); 
	
	myHasReachedEnd = true;

	FLeaderboardSubmission submission;
	submission.mySeed = myState.mySeed;
	submission.myScore = scoreTime;
	const auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.Write(submission);

	auto& blob = UMainSingelton::GetGameDB().GetBlob();
	auto blobData = blob.Get();
	blobData.mySeedData.mySeed = myState.mySeed;
	blob.Set(blobData);

	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::OUTRO);
}

void AFPController::SetEnablePawnControls(const bool aEnabled)
{
	const auto character = GetFPCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	aEnabled ?		
		character->EnableInput(this) :
		character->DisableInput(this);
}

void AFPController::EnterSection()
{
	myState.myInArena = false;
	SaveState();
	UGameplayStatics::OpenLevel(GetWorld(), "ML_Base");
}

void AFPController::EnterArena()
{
	myState.myInArena = true;
	SaveState();
	UGameplayStatics::OpenLevel(GetWorld(), "ML_Base");
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
