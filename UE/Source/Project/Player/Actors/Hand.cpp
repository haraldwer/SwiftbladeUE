#include "Hand.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHand::BeginPlay()
{
	Super::BeginPlay();

	//create dynamic material anywhere u like, Constructor or anywhere .
	if (const auto matInst = UMaterialInstanceDynamic::Create(myMaterial, this))
	{
		myMatInst = matInst;
		TArray<UStaticMeshComponent*> comps;
		GetComponents<UStaticMeshComponent>(comps);
		for (const auto& comp : comps)
			comp->SetMaterial(0, myMatInst);
	}
}

void AHand::Tick(const float aDT)
{
	Super::Tick(aDT);

	myMagicStrength = FMath::FInterpTo(myMagicStrength, 0.0f, aDT, myMagicDecrease);

	if (myMagicStrength < 0.01f)
	{
		myMagicStrength = 0.0f;
		SetActorTickEnabled(false);
	}

	// Set hand material
	if (myMatInst)
		myMatInst->SetScalarParameterValue("Strength", myMagicStrength);
}

void AHand::SetHandState(const EHandState aState) const
{
	TArray<UStaticMeshComponent*> meshes;
	GetComponents<UStaticMeshComponent>(meshes);
	for (int32 i = 0; i < meshes.Num(); i++)
		meshes[i]->SetVisibility(i == static_cast<int32>(aState));
}

void AHand::SetMagicStrength(const float aValue)
{
	if (myMagicStrength == aValue)
		return;
	SetActorTickEnabled(true);
	myMagicStrength = aValue;
}