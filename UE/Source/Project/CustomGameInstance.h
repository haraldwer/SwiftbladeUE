#pragma once

#include "CoreMinimal.h"
#include "CustomGameInstance.generated.h"

UCLASS()
class PROJECT_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

	friend class UMainSingelton;
	
public:
	virtual ~UCustomGameInstance() override = default;
	TSubclassOf<class AMenuManager> GetMenuManagerBP() const { return myMenuManagerBlueprint; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class AMenuManager> myMenuManagerBlueprint;
};
