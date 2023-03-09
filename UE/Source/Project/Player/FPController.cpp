
#include "FPController.h"

#include "FPCharacter.h"
#include "FPStateSubsystem.h"
#include "FPTime.h"
#include "GameDatabase.h"
#include "Components/Leaderboard.h"
#include "Camera/CameraActor.h"
#include "Combat/FPCombat.h"
#include "Components/Blob.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/Gameplay/GameEnd.h"
#include "Project/LevelGen/LevelRand.h"
#include "Project/LevelGen/LevelManager.h"
#include "Project/UI/Menus/MenuManager.h"
#include "Project/UI/Prompts/PromptManager.h"
#include "Project/Utility/GameUtility.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Tools/CustomCamera.h"

void AFPController::BeginPlay()
{
	Super::BeginPlay();
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::LEVEL_FADEIN);

	if (UGameUtility::IsInBaseLevel())
	{
		auto state = GetState();
		if (!state.mySeed)
			state.mySeed = FMath::Rand();
		SetState(state);

		ULevelRand::Init(state.mySeed + state.myArenaIndex * 10); 
		
		auto& levelGen = UMainSingelton::GetLevelManager();
		levelGen.GenerateLevelOrder();
		state.myInArena ?
			levelGen.LoadArena(state.myArenaIndex) :
			levelGen.LoadSection(state.myArenaIndex);
	}
}

void AFPController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SetInputMode(FInputModeGameOnly());
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

FFPState AFPController::GetState() const
{
	const auto gameInstance = GetGameInstance();
	CHECK_RETURN_LOG(!gameInstance, "GameInstance nullptr", FFPState())
	const auto subsystem = gameInstance->GetSubsystem<UFPStateSubsystem>();
	CHECK_RETURN_LOG(!subsystem, "FPStateSubsystem nullptr", FFPState())
	return subsystem->Get();
}

void AFPController::SetState(const FFPState& aState) const
{
	const auto gameInstance = GetGameInstance();
	CHECK_RETURN_LOG(!gameInstance, "GameInstance nullptr")
	const auto subsystem = gameInstance->GetSubsystem<UFPStateSubsystem>();
	CHECK_RETURN_LOG(!subsystem, "FPStateSubsystem nullptr")
	subsystem->Set(aState);
}

void AFPController::UpdateState(FFPState& aState) const
{
	const auto character = GetFPCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	if (const auto combat = character->GetCombat())
		aState.myHasSword = combat->HasSword();
	if (const auto time = character->GetTime())
		aState.myTime = time->GetScoreTime();
}

void AFPController::CharacterKilled()
{
	FFPState state = GetState();
	state.myRespawnCount++;
	SetState(state); 
	if (const auto character = GetFPCharacter())
		character->OnLivesChanged(GetRemainingLives());
	
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::DEATH);

	const auto character = GetFPCharacter();
	character->DisableInput(this);

	LOG("Character killed");

	FLeaderboardSubmission submission;
	submission.mySeed = state.mySeed;
	submission.myScore = FMath::Rand();
	const auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.Write(submission);

	auto& blob = UMainSingelton::GetGameDB().GetBlob();
	auto blobData = blob.Get();
	blobData.mySeedData.mySeed = state.mySeed;
	blob.Set(blobData);
}

int32 AFPController::GetRespawns() const
{
	return myRespawns;
}

int32 AFPController::GetRemainingLives() const
{
	return GetRespawns() - GetState().myRespawnCount;
}

void AFPController::Respawn()
{
	LOG("Respawn");
	StartTravel(EFPTravelReason::RESPAWN);
}

bool AFPController::TrySetCheckpoint(ACheckpoint& aCheckpoint)
{
	CHECK_RETURN_LOG(!aCheckpoint.IsEnabled(), "Checkpoint not yet enabled", false);
	CHECK_RETURN_LOG(myCheckpoint == &aCheckpoint, "Checkpoint already set", false);

	auto state = GetState();
	state.myRespawnCount = 0;
	state.myArenaIndex++;
	SetState(state);
	
	if (const auto character = GetFPCharacter())
		character->OnLivesChanged(GetRemainingLives());
	myCheckpoint = &aCheckpoint;
	myCheckpoint->Activate(this);
	LOG("Checkpoint set");
	
	return true;
}

void AFPController::UseCheckpoint(const ACheckpoint& aCheckpoint) const
{
	CHECK_RETURN_LOG(&aCheckpoint != myCheckpoint, "Attempted to travel to unknown checkpoint");
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::CHECKPOINT);
}

void AFPController::StartTravel(const EFPTravelReason aReason)
{
	LOG("Start travel ");
	auto state = GetState();
	state.myTravelReason = aReason;
	UpdateState(state);
	SetState(state);
	UMainSingelton::GetPromptManager().CreatePrompt(EPromptType::LEVEL_FADEOUT);
}

void AFPController::FinishTravel()
{
	auto state = GetState();
	switch (state.myTravelReason)
	{	
	case EFPTravelReason::RESPAWN:
		{
			if (state.myRespawnCount >= myRespawns)
			{
				state.myArenaIndex = 0;
				state.myRespawnCount = 0;
				state.mySeed = FMath::Rand();
				SetState(state); 
			}
			state.myInArena ?
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

void AFPController::ReachEnd(const AGameEnd& aGameEnd)
{
	CHECK_RETURN(myHasReachedEnd);
	
	const auto character = GetFPCharacter();
	CHECK_RETURN_LOG(!character, "Character nullptr");
	
	const auto time = character->GetTime();
	const auto scoreTime = time->GetScoreTime();
	CHECK_RETURN(scoreTime < myMinAllowedTime); 
	
	myHasReachedEnd = true;

	const auto state = GetState(); 
	
	FLeaderboardSubmission submission;
	submission.mySeed = state.mySeed;
	submission.myScore = scoreTime;
	const auto& lb = UMainSingelton::GetGameDB().GetLeaderboard();
	lb.Write(submission);

	auto& blob = UMainSingelton::GetGameDB().GetBlob();
	auto blobData = blob.Get();
	blobData.mySeedData.mySeed = state.mySeed;
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

void AFPController::EnterSection() const
{
	auto state = GetState();
	state.myInArena = false;
	SetState(state);
	UGameplayStatics::OpenLevel(GetWorld(), "ML_Base");
}

void AFPController::EnterArena() const
{
	auto state = GetState();
	state.myInArena = true;
	SetState(state); 
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
