#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MenuManager.generated.h"

class UMenuBase;

UENUM(BlueprintType)
enum class EMenuType : uint8
{
	UNKNOWN,
	PAUSE,
	MAIN,
	OPTIONS,
	LEADERBOARD,
	AUTHENTICATION,
	END,
	COUNT
};

UCLASS()
class PROJECT_API AMenuManager : public AActor
{
	GENERATED_BODY()

public:
	AMenuManager();
	virtual ~AMenuManager() override = default;

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
	TMap<EMenuType, TSubclassOf<class UMenuBase>> myMenuBlueprints;
	
private:
	UMenuBase* GetMenuInstance(EMenuType aMenu);
	void UpdateExclusiveVisibility();
	
	static void SetInputMode(const struct FMenuInputMode& aInputMode);
	static void ResetInputMode();
	
	UPROPERTY()
	TArray<UMenuBase*> myMenuStack;

	UPROPERTY()
	TArray<UMenuBase*> myMenus;
};
