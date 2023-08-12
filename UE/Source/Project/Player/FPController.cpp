
#include "FPController.h"

#include "FPCharacter.h"
#include "FPStateSubsystem.h"
#include "FPTime.h"
#include "GameDatabase.h"
#include "Components/Leaderboard.h"
#include "Camera/CameraActor.h"
#include "Combat/FPCombat.h"
#include "Components/Blob.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Gameplay/Checkpoint.h"
#include "Project/Gameplay/GameEnd.h"
#include "Project/LevelGen/LevelRand.h"
#include "..\LevelGen\LevelSubsystem.h"
#include "..\UI\Menus\MenuSubsystem.h"
#include "..\UI\Prompts\PromptSubsystem.h"
#include "Project/Utility/GameUtility.h"
#include "Project/Utility/Tools/CustomCamera.h"

void AFPController::BeginPlay()
{
	Super::BeginPlay();
	UPromptSubsystem::Get().CreatePrompt(EPromptType::LEVEL_FADEIN);

	if (UGameUtility::IsInBaseLevel())
	{
		auto state = UFPStateSubsystem::Get().GetState();
		if (!state.mySeed)
		{
			state.mySeed = FMath::RandRange(1, myNumSeeds);

			auto& blob = UGameDatabase::Get().GetBlob();
			auto blobData = blob.Get();
			blobData.mySeedData.mySeed = state.mySeed;
			blob.Set(blobData);
		}
		UFPStateSubsystem::Get().SetState(state);

		ULevelRand::Init(state.mySeed + state.myArenaIndex * 10); 
		
		auto& levelGen = ULevelSubsystem::Get(); 
		levelGen.GenerateLevelOrder(state.myChapter);
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
	FFPState state = UFPStateSubsystem::Get().GetState();
	state.myRespawnCount++;
	UFPStateSubsystem::Get().SetState(state); 
	if (const auto character = GetFPCharacter())
		character->OnLivesChanged(GetRemainingLives());
	
	UPromptSubsystem::Get().CreatePrompt(EPromptType::DEATH);

	const auto character = GetFPCharacter();
	character->DisableInput(this);
	
	LOG("Character killed");
}

int32 AFPController::GetRespawns() const
{
	return myRespawns;
}

int32 AFPController::GetRemainingLives() const
{
	return GetRespawns() - UFPStateSubsystem::Get().GetState().myRespawnCount;
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

	auto state = UFPStateSubsystem::Get().GetState();
	state.myRespawnCount = 0;
	state.myArenaIndex++;
	UFPStateSubsystem::Get().SetState(state);
	
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
	UPromptSubsystem::Get().CreatePrompt(EPromptType::CHECKPOINT);
}

void AFPController::StartTravel(const EFPTravelReason aReason)
{
	LOG("Start travel ");
	auto state = UFPStateSubsystem::Get().GetState();
	state.myTravelReason = aReason;
	UpdateState(state);
	UFPStateSubsystem::Get().SetState(state);
	UPromptSubsystem::Get().CreatePrompt(EPromptType::LEVEL_FADEOUT);
}

void AFPController::FinishTravel()
{
	auto state = UFPStateSubsystem::Get().GetState();
	switch (state.myTravelReason)
	{	
	case EFPTravelReason::RESPAWN:
		{
			if (state.myRespawnCount >= myRespawns)
			{
				state.myArenaIndex = 0;
				state.myRespawnCount = 0;
				state.mySeed = 0;
				UFPStateSubsystem::Get().SetState(state); 
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
	CHECK_RETURN_LOG(!time, "Time nullptr");
	
	const auto scoreTime = time->GetScoreTime();
	CHECK_RETURN(scoreTime < myMinAllowedTime); 
	
	myHasReachedEnd = true;

	auto state = UFPStateSubsystem::Get().GetState();
	state.myTime = scoreTime;
	UFPStateSubsystem::Get().SetState(state); 
	
	FLeaderboardSubmission submission;
	submission.mySeed = state.mySeed;
	submission.myScore = scoreTime;
	const auto& lb = UGameDatabase::Get().GetLeaderboard();
	lb.Write(submission);

	auto& blob = UGameDatabase::Get().GetBlob();
	auto blobData = blob.Get();
	blobData.mySeedData.mySeed = state.mySeed;
	blob.Set(blobData);

	UPromptSubsystem::Get().CreatePrompt(EPromptType::OUTRO);
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
	auto state = UFPStateSubsystem::Get().GetState();
	state.myInArena = false;
	UFPStateSubsystem::Get().SetState(state);
	UGameplayStatics::OpenLevel(GetWorld(), "ML_Base");
}

void AFPController::EnterArena() const
{
	auto state = UFPStateSubsystem::Get().GetState();
	state.myInArena = true;
	UFPStateSubsystem::Get().SetState(state); 
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
	auto& menuManager = UMenuSubsystem::Get();
	if (menuManager.IsAnyMenuOpen())
	{
		if (!menuManager.IsMenuTop(EMenuType::MAIN))
			menuManager.PopMenu();
	}
	else
		menuManager.OpenMenu(EMenuType::PAUSE);
}
