
#include "FPCharacter.h"

#include "FPCamera.h"
#include "FPController.h"
#include "Effects/FPPostProcessing.h"
#include "Actors/Hand.h"
#include "Actors/Sword.h"
#include "Animation/FPAnimatorNew.h"
#include "Animation/States/FPAnimationStateDeath.h"
#include "Blueprint/UserWidget.h"
#include "Combat/FPCombat.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Movement/FPMovement.h"
#include "Project/Gameplay/Door.h"
#include "Project/Utility/EngineUtility.h"
#include "Project/Utility/MainSingelton.h"
#include "Project/Utility/Utility.h"
#include "Project/Utility/Tools/CustomCamera.h"
#include "Project/Utility/Tools/Effect.h"

AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	if(const auto movement = GetCharacterMovement())
		movement->NavAgentProps.bCanCrouch = true;

	// Setup components
	
	myCamera = CreateDefaultSubobject<UCustomCamera>("FirstPersonCamera");
	CHECK_ASSERT(!myCamera, "Failed to create camera component");
	myCamera->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	myCamera->bUsePawnControlRotation = true;
	
	myFPCamera = CreateDefaultSubobject<UFPCamera>("FPCamera");
	CHECK_ASSERT(!myFPCamera, "Failed to create FPCamera component");
	
	myFPMovement = CreateDefaultSubobject<UFPMovement>("FPMovement");
	CHECK_ASSERT(!myFPMovement, "Failed to create movement component");
	//myFPMovement->SetupAttachment(RootComponent);
	
	myFPAnimator = CreateDefaultSubobject<UFPAnimatorNew>("FPAnimator");
	CHECK_ASSERT(!myFPAnimator, "Failed to create animator component");
	//myFPAnimator->SetupAttachment(RootComponent);
	
	myFPCombat = CreateDefaultSubobject<UFPCombat>("FPCombat");
	CHECK_ASSERT(!myFPCombat, "Failed to create combat component");
	//myFPCombat->SetupAttachment(RootComponent);

	myPP = CreateDefaultSubobject<UFPPostProcessing>("FPPostProcessing");
	CHECK_ASSERT(!myPP, "Failed to create pp component");
	
	myInteractionCollider = CreateDefaultSubobject<USphereComponent>("InteractionCollider");
	CHECK_ASSERT(!myInteractionCollider, "Failed to create interaction collider");
	myInteractionCollider->SetupAttachment(myCamera);

	const auto capsule = GetCapsuleComponent();
	CHECK_ASSERT(!capsule, "No capsule component");
	myWallDetection = CreateDefaultSubobject<UCapsuleComponent>("WallDetection");
	CHECK_ASSERT(!myWallDetection, "Failed to create wall detection component");
	float radius, halfHeight;
	capsule->GetUnscaledCapsuleSize(radius, halfHeight);
	myWallDetection->InitCapsuleSize(radius + 10, halfHeight + 10);
	myWallDetection->SetupAttachment(RootComponent);
	myWallDetection->SetGenerateOverlapEvents(true);
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
		}
	}

	if (UEngineUtility::IsInBaseLevel())
	{
		if (const auto controller = GetFPController())
			controller->LoadState();	
	}
	else
	{
		// Just give player a sword
		if (myFPCombat)
			myFPCombat->SetHasSword(true);
	}
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
	
	const float lowestPoint = UMainSingelton::GetLevelGenerator().GetLowestEnd();
	if (GetActorLocation().Z < lowestPoint + myKillZ)
		Die("OutOfBounds"); 
	
	if (myFPCamera)
		myFPCamera->BeginTick(DeltaTime);
}

void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	LOG("Binding input");
	
	InputComponent->BindAxis("MoveHorizontal", myFPMovement, &UFPMovement::MoveHorizontal);
	InputComponent->BindAxis("MoveVertical", myFPMovement, &UFPMovement::MoveVertical);
	InputComponent->BindAxis("LookHorizontal", myFPMovement, &UFPMovement::LookHorizontal);
	InputComponent->BindAxis("LookVertical", myFPMovement, &UFPMovement::LookVertical);
	InputComponent->BindAction("Jump", IE_Pressed, myFPMovement, &UFPMovement::JumpPressed);
	InputComponent->BindAction("Jump", IE_Released, myFPMovement, &UFPMovement::JumpReleased);
	InputComponent->BindAction("Crouch", IE_Pressed, myFPMovement, &UFPMovement::CrouchPressed);
	InputComponent->BindAction("Crouch", IE_Released, myFPMovement, &UFPMovement::CrouchReleased);
	InputComponent->BindAction("Dash", IE_Pressed, myFPMovement, &UFPMovement::Dash);
	InputComponent->BindAction("Grapple", IE_Pressed, myFPMovement, &UFPMovement::Grapple);

	CHECK_RETURN_LOG(!myFPCombat, "Combat not set");

	InputComponent->BindAction("Interact", IE_Pressed, myFPCombat, &UFPCombat::Interact);
	InputComponent->BindAction("Strike", IE_Pressed, myFPCombat, &UFPCombat::Strike);
	InputComponent->BindAction("Block", IE_Pressed, myFPCombat, &UFPCombat::Block); 
}

AFPController* AFPCharacter::GetFPController() const
{
	return Cast<AFPController>(GetController());
}

ASword* AFPCharacter::GetSword() const
{
	return myFPCombat ? myFPCombat->GetSword() : nullptr;
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
	if (myFPCombat->TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser))
		Die(aDamageCauser ? aDamageCauser->GetName() : "");
	return Super::TakeDamage(aDamageAmount, aDamageEvent, aEventInstigator, aDamageCauser);
}

void AFPCharacter::Die(const FString& anObjectName)
{
	CHECK_RETURN(!myAlive);
	myAlive = false;
	if (const auto animator = GetAnimator())
		animator->SetState<UFPAnimationStateDeath>();
	const auto controller = GetFPController();
	CHECK_RETURN_LOG(!controller, "Controller nullptr");
	controller->CharacterKilled();
}

void AFPCharacter::OnLivesChanged(const int32 aNewLifeCount) const
{
	if (const auto sword = GetSword())
		sword->SetCrystalsActive(aNewLifeCount, true);
}

void AFPCharacter::DoorOpened(ADoor* aDoor) const
{
	// Arena door
	const auto controller = GetFPController();
	CHECK_RETURN_LOG(!controller, "No controller");
	controller->StartTravel(EFPTravelReason::ARENA);
}

bool AFPCharacter::HasMagic() const
{
	if (const auto combat = GetCombat())
		return combat->HasSword();
	return false;
}

void AFPCharacter::OnStartCrouch(const float aHalfHeightAdjust, const float aScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(aHalfHeightAdjust, aScaledHalfHeightAdjust);
	if (myFPAnimator)
		myFPAnimator->OnCrouch(true, aHalfHeightAdjust);
}

void AFPCharacter::OnEndCrouch(const float aHalfHeightAdjust, const float aScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(aHalfHeightAdjust, aScaledHalfHeightAdjust);
	if (myFPAnimator)
		myFPAnimator->OnCrouch(false, aHalfHeightAdjust);
}

void AFPCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                         FVector NormalImpulse, const FHitResult& Hit)
{
	if (myFPMovement)
		myFPMovement->OnHit(Hit);
}

void AFPCharacter::Landed(const FHitResult& aHit)
{
	Super::Landed(aHit);
	if (myFPMovement)
		myFPMovement->OnLanded();
}