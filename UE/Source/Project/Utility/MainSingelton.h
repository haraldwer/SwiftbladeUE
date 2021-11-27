#pragma once

#include "CoreMinimal.h"
#include "MainSingelton.generated.h"

UCLASS()
class PROJECT_API UMainSingelton : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual ~UMainSingelton() override = default;
	
	UFUNCTION(BlueprintCallable, Category="MainSingelton Getter")
	static class AMenuManager* GetMenuManger();

	UFUNCTION(BlueprintCallable, Category="MainSingelton Getter")
	static class AFPController* GetLocalController();
	
	UFUNCTION(BlueprintCallable, Category="MainSingelton Getter")
	static class AFPCharacter* GetLocalPlayer();

	static class ACustomGameMode* GetGameMode();
	static class UCustomGameInstance* GetGameInstance(); 
	
};
