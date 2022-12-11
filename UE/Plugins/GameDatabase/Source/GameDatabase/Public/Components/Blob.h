#pragma once

#include "DBComponent.h"
#include "BlobData.h"
#include "Blob.generated.h"

UCLASS(ClassGroup=(GameDB))
class GAMEDATABASE_API UBlob : public UDBComponent
{
	GENERATED_BODY()

public:

	void Init();

	void Set(const FBlobData& someData);
	const FBlobData& Get() const { return myData; }

private:

	UFUNCTION()
	void OnReadSuccess(const TArray<FNakamaStoreObjectData>& someObjects);
	UFUNCTION()
	void OnReadError(const FNakamaError& anError);

	UFUNCTION()
	void OnWriteSuccess(const FNakamaStoreObjectAcks& someObjects);
	UFUNCTION()
	void OnWriteError(const FNakamaError& anError);
	
	
	FBlobData myData;
	
};
