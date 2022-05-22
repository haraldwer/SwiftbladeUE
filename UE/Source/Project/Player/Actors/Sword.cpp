#include "Sword.h"

#include "Project/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Project/Player/FPController.h"

// Sets default values
ASword::ASword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

TArray<AActor*> ASword::GetOverlaps(UClass* aClass) const
{
	TArray<AActor*> result;

	if (!myPlayer.IsValid())
		return result;
	
	GetOverlappingActors(result, aClass);
	return result;
}

void ASword::SetPlayer(AFPCharacter* aPlayer)
{
	if (!aPlayer)
	{
		Return();
		return;
	}
	
	AttachToActor(aPlayer, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	myPlayer = aPlayer;
}

void ASword::Return()
{
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	myPlayer = nullptr;
}

void ASword::CreateHitEffect(const AActor* anActor) const
{
	CHECK_RETURN(!myPlayer.IsValid());
	FTransform trans = GetTransform();
	
	const FVector base = trans.GetLocation() + GetActorForwardVector() * 20.0f;
	const FVector start = base + GetActorUpVector() * myEffectStartOffset;
	const FVector end = base + GetActorUpVector() * myEffectEndOffset;
	const FVector point = anActor ?
		FMath::ClosestPointOnLine(start, end, anActor->GetActorLocation()) :
		FMath::Lerp(start, end, 0.5f);
	trans.SetLocation(point);
	
	myPlayer->CreateEffect(myHitEffectBP, trans);
}

// Called when the game starts or when spawned
void ASword::BeginPlay()
{
	Super::BeginPlay();
	myStartLocation = GetActorLocation();
	myTimer = 0;
	myPlayer.Reset();
}

// Called every frame
void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!myPlayer.IsValid())
	{
		myTimer += DeltaTime;
		SetActorLocationAndRotation(
			myStartLocation + FVector(0, 0, FMath::Sin(myTimer * myPositionSpeed) * myPositionScale), 
			FRotator(0, myTimer * myRotationSpeed + 90, 0));
	}
}

