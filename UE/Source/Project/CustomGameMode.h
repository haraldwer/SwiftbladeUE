#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

UCLASS()
class PROJECT_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

	friend class UMainSingelton;
	
public:
	virtual void InitGameState() override;

private:
	void CreateMenuManager();
	void CreatePromptManager();
	void CreateEnemyManager();
	void CreateLevelGenerator();
	
	UPROPERTY();
	class AMenuManager* myMenuManager;
	UPROPERTY();
	class APromptManager* myPromptManager;
	UPROPERTY();
	class AEnemyManager* myEnemyManager;
	UPROPERTY();
	class ALevelGenerator* myLevelGenerator;
	
};
