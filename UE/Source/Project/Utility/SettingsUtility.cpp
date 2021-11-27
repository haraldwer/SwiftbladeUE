// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsUtility.h"

#include "GameFramework/GameUserSettings.h"

void USettingsUtility::SetResolutionScale(const int32 aResolution)
{
	const UGameUserSettings* settings = GEngine->GameUserSettings.Get();
	const auto resolution = settings->GetScreenResolution();
	const float height = resolution.Y;
	const float screenPercentage = (static_cast<float>(aResolution) / height) * 100.0f;
	GEngine->Exec(GEngine->GetWorld(), *FString("r.ScreenPercentage " + FString::SanitizeFloat(screenPercentage)));
	GEngine->Exec(GEngine->GetWorld(), *FString("r.Upscale.Quality 0"));
}

void USettingsUtility::SetFeatureEnabled(ERenderFeature aFeature, const bool aEnabled)
{
#ifdef UE_BUILD_SHIPPING
#endif
	
	const auto featureName = RenderFeatureCommands[static_cast<int32>(aFeature)];
	LOG(featureName);
	GEngine->Exec(GEngine->GetWorld(), *FString(featureName + " " + FString::FromInt(aEnabled)));
}

void USettingsUtility::UnlockFPS()
{
	GEngine->Exec(GEngine->GetWorld(), TEXT("t.MaxFPS 0"));
	GEngine->Exec(GEngine->GetWorld(), TEXT("r.VSync 0"));
}