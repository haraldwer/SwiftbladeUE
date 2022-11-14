#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

UCLASS()
class PROJECT_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

	friend class UMainSingelton;
	
public:
	
	virtual ~UCustomGameInstance() override = default;
	TSubclassOf<class AMenuManager> GetMenuManagerBP() const { return myMenuManagerBlueprint; }
	TSubclassOf<class APromptManager> GetPromptManagerBP() const { return myPromptManagerBlueprint; }
	TSubclassOf<class AEnemyManager> GetEnemyManagerBP() const { return myEnemyManagerBlueprint; }
	TSubclassOf<class ALevelManager> GetLevelGeneratorBP() const { return myLevelGeneratorBlueprint; }
	TSubclassOf<class AGameDB> GetGameDBBP() const { return myGameDBBlueprint; }

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class AMenuManager> myMenuManagerBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class APromptManager> myPromptManagerBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="Enemies")
	TSubclassOf<class AEnemyManager> myEnemyManagerBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="Levels")
	TSubclassOf<class ALevelManager> myLevelGeneratorBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="Game Database")
	TSubclassOf<class AGameDB> myGameDBBlueprint;

};
