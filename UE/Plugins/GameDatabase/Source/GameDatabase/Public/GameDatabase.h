#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameDB, Log, All);

class FGameDatabase : public IModuleInterface
{

public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void Open();
	void Close();

	// For simply uploading objects
	void Upload(const FString& aKey);
	// For more advanced queries
	void Query(const FString& aKey);

private:

	static std::string WideToNarrow(const FString& aString);
	static FString NarrowToWide(const std::string& aString);

	const FString myConnectionString = "Server=DESKTOP-RBP1N0G\\GAMEDB;Database=GameDB;User Id=SwiftbladeUser;Password=BurningSword241;";
	float myTimeoutDuration = 2.0f;
};
