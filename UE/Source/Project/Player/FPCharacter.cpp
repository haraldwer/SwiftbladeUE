
#include "FPCharacter.h"

#include "FPCombat.h"
#include "FPController.h"
#include "FPMovement.h"
#include "Actors/Hand.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Animation/FPAnimator.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Project/Effect.h"
#include "Project/Utility.h"

AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	const auto movement = GetCharacterMovement();
	if(movement)
		movement->NavAgentProps.bCanCrouch = true;
	
	myCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	CHECK_ASSERT(!myCamera, "Failed to create camera component");
	myCamera->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	myCamera->bUsePawnControlRotation = true;
		
	myFPMovement = CreateDefaultSubobject<UFPMovement>("FPMovement");
	CHECK_ASSERT(!myFPMovement, "Failed to create movement component");
	
	myFPAnimator = CreateDefaultSubobject<UFPAnimator>("FPAnimator");
	CHECK_ASSERT(!myFPAnimator, "Failed to create animator component");
	
	myFPCombat = CreateDefaultSubobject<UFPCombat>("FPCombat");
	CHECK_ASSERT(!myFPCombat, "Failed to create combat component");

	if(const auto capsule = GetCapsuleComponent())
	{
		myWallDetection = CreateDefaultSubobject<UCapsuleComponent>("WallDetection");
		CHECK_ASSERT(!myWallDetection, "Failed to create wall detection component");
		float radius, halfHeight;
		capsule->GetUnscaledCapsuleSize(radius, halfHeight);
		myWallDetection->InitCapsuleSize(radius + 10, halfHeight + 10);
		myWallDetection->SetupAttachment(RootComponent);
		myWallDetection->SetGenerateOverlapEvents(true);
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
		myFullHeight = capsule->GetUnscaledCapsuleHalfHeight();
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
			myLeftHand->SetActorRelativeScale3D(FVector(1, -1, 1));
		}		
	}

	const auto controller = GetFPController();
	if (controller)
		controller->CharacterCreated(this);
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

AFPController* AFPCharacter::GetFPController() const
{
	return Cast<AFPController>(GetController());
}

void AFPCharacter::TickActor(float DeltaTime, ELevelTick Tick, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, Tick, ThisTickFunction);

	myCamera->ClearAdditiveOffset();
}

void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	LOG("Binding input");
	
	InputComponent->BindAxis("MoveHorizontal", this, &AFPCharacter::MoveHorizontal);
	InputComponent->BindAxis("MoveVertical", this, &AFPCharacter::MoveVertical);
	InputComponent->BindAxis("LookHorizontal", this, &AFPCharacter::LookHorizontal);
	InputComponent->BindAxis("LookVertical", this, &AFPCharacter::LookVertical);
	
	CHECK_RETURN_LOG(!myFPMovement, "Movement not set");
	
	InputComponent->BindAction("Jump", IE_Pressed, myFPMovement, &UFPMovement::PressJump);
	InputComponent->BindAction("Jump", IE_Released, myFPMovement, &UFPMovement::ReleaseJump);
	InputComponent->BindAction("Crouch", IE_Pressed, myFPMovement, &UFPMovement::PressCrouch);
	InputComponent->BindAction("Crouch", IE_Released, myFPMovement, &UFPMovement::ReleaseCrouch);
	InputComponent->BindAction("Dash", IE_Pressed, myFPMovement, &UFPMovement::Dash);
	InputComponent->BindAction("Grapple", IE_Pressed, myFPMovement, &UFPMovement::Grapple);

	CHECK_RETURN_LOG(!myFPCombat, "Combat not set");
	
	InputComponent->BindAction("Strike", IE_Pressed, myFPCombat, &UFPCombat::Strike);
}

void AFPCharacter::MoveHorizontal(const float aValue)
{
	AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y), aValue);
}

void AFPCharacter::MoveVertical(const float aValue)
{
	auto rot = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	rot.Z = 0.0f;
	rot.Normalize();
	AddMovementInput(rot, aValue);
}

void AFPCharacter::LookHorizontal(const float aValue)
{
	AddControllerYawInput(aValue * mySensitivity);
}

void AFPCharacter::LookVertical(const float aValue)
{
	AddControllerPitchInput(aValue * mySensitivity * -1.0f);
}

void AFPCharacter::Landed(const FHitResult& aHit)
{
	Super::Landed(aHit);
	if(myFPMovement)
		myFPMovement->Landed(aHit);
}

void AFPCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                         FVector NormalImpulse, const FHitResult& Hit)
{
	const auto c = GetMovementComponent();
	if (c && myFPMovement && c->IsFalling())
		myFPMovement->StartWallrun(Hit.ImpactNormal);
}

void AFPCharacter::Die(const FString& anObjectName)
{
	const auto controller = GetFPController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr");
	controller->CharacterKilled();
}

UCameraComponent* AFPCharacter::GetCamera() const
{
	return myCamera;
}

UFPAnimator* AFPCharacter::GetAnimator() const
{
	return myFPAnimator;
}

UFPMovement* AFPCharacter::GetMovement() const
{
	return myFPMovement;
}

UFPCombat* AFPCharacter::GetCombat() const
{
	return myFPCombat;
}

UCapsuleComponent* AFPCharacter::GetWallDetection() const
{
	return myWallDetection;
}

AHand* AFPCharacter::GetRightHand() const
{
	return myLeftHand;
}

AHand* AFPCharacter::GetLeftHand() const
{
	return myRightHand;
}

void AFPCharacter::SetHalfHeight()
{
	const auto capsule = GetCapsuleComponent();
	if (!capsule)
		return;
	if (capsule->GetUnscaledCapsuleHalfHeight() == myFullHeight * 0.5f)
		return;
	capsule->SetCapsuleHalfHeight(myFullHeight * 0.5f, false);
	auto l = GetActorLocation();
	l.Z -= myFullHeight * 0.5f;
	SetActorLocation(l);
}

void AFPCharacter::SetFullHeight()
{
	const auto capsule = GetCapsuleComponent();
	if (!capsule)
		return;
	if (capsule->GetUnscaledCapsuleHalfHeight() == myFullHeight)
		return;
	capsule->SetCapsuleHalfHeight(myFullHeight, false);
	auto l = GetActorLocation();
	l.Z += myFullHeight * 0.5f;
	SetActorLocation(l);
}

AEffect* AFPCharacter::CreateEffect(const TSubclassOf<AEffect>& aBP, const FTransform& aTransform)
{
	const auto bp = aBP.Get();
	CHECK_RETURN_LOG(!bp, "Effect BP not set", nullptr);
	AActor* actor = GetWorld()->SpawnActor(aBP.Get());
	CHECK_RETURN_LOG(!actor, "Failed to create effect", nullptr);
	AEffect* effect = Cast<AEffect>(actor);
	CHECK_RETURN_LOG(!effect, "Effect not of type AEffect", nullptr)
	effect->SetActorTransform(aTransform);
	return effect;
}
