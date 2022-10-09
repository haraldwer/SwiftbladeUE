#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Breakable.generated.h"

UCLASS(ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent))
class PROJECT_API UBreakable : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBreakable();

	UFUNCTION(BlueprintCallable)
	void Break(const FVector& aPosition, const FVector& aNormal);
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void BreakMeshes(const FVector& aPosition, const FVector& aNormal);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool myDestroyOnBreak = false;

	// Note: Requires CPU Access for all meshes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool mySplit = false;
	
};
