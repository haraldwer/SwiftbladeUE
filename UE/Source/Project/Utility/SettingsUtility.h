
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SettingsUtility.generated.h"

UENUM(BlueprintType)
enum class ERenderFeature : uint8
{
	LUMEN,
	FOG,
	TONEMAPPER_GRAIN,
	COUNT
};

const FString RenderFeatureCommands[]
{
	FString("r.Lumen.DiffuseIndirect.Allow"),
	FString("r.VolumetricFog"),
	FString("r.Tonemapper.GrainQuantization")
};

UCLASS()
class PROJECT_API USettingsUtility : public UObject
{
	GENERATED_BODY()

public:
	
	virtual ~USettingsUtility() override = default;

	UFUNCTION(BlueprintCallable, Category="Settings utility functions")
	static int32 GetGameResolution() { return 320; }

	UFUNCTION(BlueprintCallable, Category="Settings utility functions")
	static void SetResolutionScale(int32 aResolution);

	UFUNCTION(BlueprintCallable, Category="Settings utility functions")
	static void SetFeatureEnabled(ERenderFeature aFeature, bool aEnabled);

	UFUNCTION(BlueprintCallable, Category="Settings utility functions")
	static void UnlockFPS();
};
