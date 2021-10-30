
#include "FPCharacter.h"

#include "FPCombat.h"
#include "FPMovement.h"
#include "Actors/Hand.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Animation/FPAnimator.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	if (!myCamera)
	{
		myCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
		check(myCamera != nullptr);
		myCamera->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
		myCamera->bUsePawnControlRotation = true;
	}

	if (!myAnimator)
	{
		myAnimator = CreateDefaultSubobject<UFPAnimator>("Animator");
		check(myAnimator != nullptr);
	}

	if (!myMovement)
	{
		myMovement = CreateDefaultSubobject<UFPMovement>("Movement");
		check(myMovement != nullptr);
	}

	if (!myCombat)
	{
		myCombat = CreateDefaultSubobject<UFPCombat>("Combat");
		check(myCombat != nullptr);
	}
}

void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();

	const auto capsule = GetCapsuleComponent();
	if(capsule)
	{
		capsule->OnComponentHit.RemoveDynamic(this, &AFPCharacter::OnHit);
		capsule->OnComponentHit.AddDynamic(this, &AFPCharacter::OnHit);
	}

	if (myHandBP)
	{
		if(!myRightHand)
		{
			FActorSpawnParameters params;
			myRightHand = GetWorld()->SpawnActor<AHand>(myHandBP, params);
			myRightHand->AttachToActor(this, 
				FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		}

		if(!myLeftHand)
		{
			FActorSpawnParameters params;
			myLeftHand = GetWorld()->SpawnActor<AHand>(myHandBP, params);
			myLeftHand->AttachToActor(this,
				FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			myLeftHand->SetActorRelativeScale3D(FVector(-1, 1, 1));
		}		
	}
}

void AFPCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	const auto capsule = GetCapsuleComponent();
	if(capsule)
		capsule->OnComponentHit.RemoveDynamic(this, &AFPCharacter::OnHit);
	
	if(myRightHand)
		myRightHand->Destroy();
	if(myLeftHand)
		myLeftHand->Destroy();
}

void AFPCharacter::TickActor(float DeltaTime, ELevelTick Tick, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, Tick, ThisTickFunction);
}

void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveHorizontal", this, &AFPCharacter::MoveHorizontal);
	InputComponent->BindAxis("MoveVertical", this, &AFPCharacter::MoveVertical);
	InputComponent->BindAxis("LookHorizontal", this, &AFPCharacter::LookHorizontal);
	InputComponent->BindAxis("LookVertical", this, &AFPCharacter::LookVertical);

	InputComponent->BindAction("Jump", IE_Pressed, myMovement, &UFPMovement::PressJump);
	InputComponent->BindAction("Jump", IE_Released, myMovement, &UFPMovement::ReleaseJump);
	InputComponent->BindAction("Dash", IE_Pressed, myMovement, &UFPMovement::Dash);
	InputComponent->BindAction("Grapple", IE_Pressed, myMovement, &UFPMovement::Grapple);
	
	InputComponent->BindAction("Strike", IE_Pressed, myCombat, &UFPCombat::Strike);
}

void AFPCharacter::MoveHorizontal(float aValue)
{
	AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y), aValue);
}

void AFPCharacter::MoveVertical(float aValue)
{
	auto rot = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	rot.Z = 0.0f;
	rot.Normalize();
	AddMovementInput(rot, aValue);
}

void AFPCharacter::LookHorizontal(float aValue)
{
	AddControllerYawInput(aValue * mySensitivity);
}

void AFPCharacter::LookVertical(float aValue)
{
	AddControllerPitchInput(aValue * mySensitivity * -1.0f);
}

void AFPCharacter::Landed(const FHitResult& aHit)
{
	Super::Landed(aHit);
	if(myMovement)
		myMovement->Landed(aHit);
}

void AFPCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                         FVector NormalImpulse, const FHitResult& Hit)
{
	const auto c = GetMovementComponent();
	if (c && myMovement && c->IsFalling())
		myMovement->StartWallrun(Hit.ImpactNormal);
}

void AFPCharacter::Die(FString anObjectName)
{
	if (myDiedThisFrame)
		return;
	myDiedThisFrame = true;
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "You died: " + anObjectName);
	if(!myHasCheckpoint)
	{
		const auto controller = GetController();
		Destroy();
		GetWorld()->GetAuthGameMode()->RestartPlayer(controller);
	}
	else
	{
		SetActorLocationAndRotation(myCheckpointLocation, FQuat(), false, nullptr, ETeleportType::TeleportPhysics);
		myRespawnCount++;
		if (myRespawnCount > myRespawns)
			myHasCheckpoint = false;
	}
}

void AFPCharacter::SetCheckpoint(AActor* aActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Checkpoint Set");
	myHasCheckpoint = true;
	myRespawnCount = 0;
	myCheckpointLocation = GetActorLocation();
}

UCameraComponent* AFPCharacter::GetCamera() const
{
	return myCamera;
}

UFPAnimator* AFPCharacter::GetAnimator() const
{
	return myAnimator;
}

UFPMovement* AFPCharacter::GetMovement() const
{
	return myMovement;
}

UFPCombat* AFPCharacter::GetCombat() const
{
	return myCombat;
}

AHand* AFPCharacter::GetRightHand() const
{
	return myLeftHand;
}

AHand* AFPCharacter::GetLeftHand() const
{
	return myRightHand;
}
