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
	TSubclassOf<class ALevelGenerator> GetLevelGeneratorBP() const { return myLevelGeneratorBlueprint; }

	void AddPersistentObject(UObject* anObject);
	void RemovePersistentObject(UObject* anObject);

	template <class T>
	T* GetPersistentObject() const
	{
		for (auto& it : myPersistentObjects)
			if (auto ptr = Cast<T>(it))
				return ptr;
		return nullptr; 
	}
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class AMenuManager> myMenuManagerBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class APromptManager> myPromptManagerBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class AEnemyManager> myEnemyManagerBlueprint;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class ALevelGenerator> myLevelGeneratorBlueprint;

private:
	UPROPERTY()
	TArray<UObject*> myPersistentObjects;
};
