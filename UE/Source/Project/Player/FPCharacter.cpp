
#include "FPCharacter.h"

#include "FPCamera.h"
#include "FPCombat.h"
#include "FPController.h"
#include "Movement/FPMovementStateMachine.h"
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
#include "Project/Utility/Tools/Effect.h"
#include "Project/Utility/Utility.h"

AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	if(const auto movement = GetCharacterMovement())
		movement->NavAgentProps.bCanCrouch = true;

	// Setup components
	
	myCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	CHECK_ASSERT(!myCamera, "Failed to create camera component");
	myCamera->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	myCamera->bUsePawnControlRotation = true;

	myFPCamera = CreateDefaultSubobject<UFPCamera>("FPCamera");
	CHECK_ASSERT(!myFPCamera, "Failed to create FPCamera component");
	
	myFPMovement = CreateDefaultSubobject<UFPMovementStateMachine>("FPMovementStateMachine");
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

	// Add collider callbacks
	if(const auto capsule = GetCapsuleComponent())
	{
		capsule->OnComponentHit.RemoveDynamic(this, &AFPCharacter::OnHit);
		capsule->OnComponentHit.AddDynamic(this, &AFPCharacter::OnHit);
		myFullHeight = capsule->GetUnscaledCapsuleHalfHeight();
	}

	// Setup hands
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

	if (const auto controller = GetFPController())
		controller->CharacterCreated(this);
}

void AFPCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	// Remove collision callbacks
	if(const auto capsule = GetCapsuleComponent())
		capsule->OnComponentHit.RemoveDynamic(this, &AFPCharacter::OnHit);

	// Destroy hands
	if(myRightHand)
		myRightHand->Destroy();
	if(myLeftHand)
		myLeftHand->Destroy();
}

void AFPCharacter::TickActor(float DeltaTime, ELevelTick Tick, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, Tick, ThisTickFunction);
	if (myFPCamera)
		myFPCamera->BeginTick(DeltaTime);
}

void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	LOG("Binding input");
	
	InputComponent->BindAxis("MoveHorizontal", myFPMovement, &UFPMovementStateMachine::MoveHorizontal);
	InputComponent->BindAxis("MoveVertical", myFPMovement, &UFPMovementStateMachine::MoveVertical);
	InputComponent->BindAxis("LookHorizontal", myFPMovement, &UFPMovementStateMachine::LookHorizontal);
	InputComponent->BindAxis("LookVertical", myFPMovement, &UFPMovementStateMachine::LookVertical);
	InputComponent->BindAction("Jump", IE_Pressed, myFPMovement, &UFPMovementStateMachine::JumpPressed);
	InputComponent->BindAction("Jump", IE_Released, myFPMovement, &UFPMovementStateMachine::JumpReleased);
	InputComponent->BindAction("Crouch", IE_Pressed, myFPMovement, &UFPMovementStateMachine::CrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, myFPMovement, &UFPMovementStateMachine::CrouchReleased);
	InputComponent->BindAction("Dash", IE_Pressed, myFPMovement, &UFPMovementStateMachine::Dash);
	InputComponent->BindAction("Grapple", IE_Pressed, myFPMovement, &UFPMovementStateMachine::Grapple);

	CHECK_RETURN_LOG(!myFPCombat, "Combat not set");

	InputComponent->BindAction("Interact", IE_Pressed, myFPCombat, &UFPCombat::Interact);
	InputComponent->BindAction("Strike", IE_Pressed, myFPCombat, &UFPCombat::Strike);
	InputComponent->BindAction("Block", IE_Pressed, myFPCombat, &UFPCombat::Block); 
}

AFPController* AFPCharacter::GetFPController() const
{
	return Cast<AFPController>(GetController());
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

AEffect* AFPCharacter::CreateEffect(const TSubclassOf<AEffect>& aBP, const FTransform& aTransform) const
{
	const auto bp = aBP.Get();
	CHECK_RETURN_LOG(!bp, "Effect BP not set", nullptr);
	AActor* actor = GetWorld()->SpawnActor(bp);
	CHECK_RETURN_LOG(!actor, "Failed to create effect", nullptr);
	AEffect* effect = Cast<AEffect>(actor);
	CHECK_RETURN_LOG(!effect, "Effect not of type AEffect", nullptr)
	effect->SetActorTransform(aTransform);
	return effect;
}

float AFPCharacter::TakeDamage(float aDamageAmount, FDamageEvent const& aDamageEvent, AController* aEventInstigator,
	AActor* aDamageCauser)
{
	LOG("Took damage"); 
	return Super::TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser);
}

void AFPCharacter::Die(const FString& anObjectName)
{
	if (const auto combat = GetCombat())
		combat->ReturnSword();
	
	const auto controller = GetFPController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr");
	controller->CharacterKilled();
}

void AFPCharacter::OnRespawned()
{
	LOG("OnRespawned");
}

void AFPCharacter::Landed(const FHitResult& aHit)
{
	Super::Landed(aHit);
	if (myFPMovement)
		myFPMovement->OnLanded();
}

void AFPCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
						 FVector NormalImpulse, const FHitResult& Hit)
{
	if (myFPMovement)
		myFPMovement->OnHit(Hit);
}