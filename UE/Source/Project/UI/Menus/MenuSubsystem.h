#pragma once

#include "CoreMinimal.h"
#include "MenuSubsystem.generated.h"

struct FMenuInputMode;
class UMenuBase;

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	UNKNOWN,
	PAUSE,
	MAIN,
	PLAY,
	OPTIONS,
	LEADERBOARD,
	AUTHENTICATION,
	END,
	COUNT
};

UCLASS()
class PROJECT_API UMenuSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	static UMenuSubsystem& Get(); 

	UFUNCTION(BlueprintCallable)
	void ToggleMenu(EMenuType aMenu);
	UFUNCTION(BlueprintCallable)
	void OpenMenu(EMenuType aMenu);
	UFUNCTION(BlueprintCallable)
	void CloseMenu(EMenuType aMenu);
	UFUNCTION(BlueprintCallable)
	void PopMenu();
	UFUNCTION(BlueprintCallable)
	void CloseAll();
	UFUNCTION(BlueprintCallable)
	bool IsMenuOpen(EMenuType aMenu) const;
	UFUNCTION(BlueprintCallable)
	bool IsMenuTop(EMenuType aMenu) const;
	UFUNCTION(BlueprintCallable)
	bool IsAnyMenuOpen() const;

	// Get menu from stack
	UFUNCTION(BlueprintCallable)
	UMenuBase* GetMenu(EMenuType aMenu);

	// Get const menu from stack
	UFUNCTION(BlueprintCallable)
	const UMenuBase* GetMenuConst(EMenuType aMenu) const;

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<EMenuType, TSubclassOf<UMenuBase>> myMenuBlueprints;
	
private:
	UMenuBase* GetMenuInstance(EMenuType aMenu);
	void UpdateExclusiveVisibility();
	
	static void SetInputMode(const FMenuInputMode& aInputMode);
	static void ResetInputMode();
	
	UPROPERTY()
	TArray<UMenuBase*> myMenuStack;

	UPROPERTY()
	TArray<UMenuBase*> myMenus;
};
