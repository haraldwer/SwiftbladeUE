#include "Components/Blob.h"
#include "GameDBModule.h"
#include "JsonObjectConverter.h"
#include "NakamaClient.h"
#include "NakamaSession.h"

void UBlob::Init()
{
	const auto client = GetClient();
	const auto session = GetSession();
	if (!client || !session)
	{
		UE_LOG(LogGameDB, Error, TEXT("GameDB not yet connected on blob init")); 
		return;
	}
	
	FOnStorageObjectsRead success;
	success.AddUniqueDynamic(this, &UBlob::OnReadSuccess);
	FOnError error;
	error.AddUniqueDynamic(this, &UBlob::OnReadError);

	TArray<FNakamaReadStorageObjectId> objects;
	for (TFieldIterator<UProperty> itr(myData.StaticStruct()); itr; ++itr)
	{
		const auto property = Cast<UStructProperty>(*itr);
		if (!property)
			continue; // Not a struct
		
		FString variableName = property->GetName();
		auto& obj = objects.Emplace_GetRef(); 
		obj.Collection = "blob";
		obj.Key = variableName; 
		obj.UserId = session->SessionData.UserId;
	}
	
	client->ReadStorageObjects(
		session,
		objects,
		success,
		error);
}

void UBlob::OnReadSuccess(const TArray<FNakamaStoreObjectData>& someObjects)
{
	UE_LOG(LogGameDB, Log, TEXT("Blob read success"));

	const auto dataStruct = myData.StaticStruct();
	for (auto& object : someObjects)
	{
		FString propertyName = object.Key;
		FString json = object.Value;

		const auto property = Cast<UStructProperty>(dataStruct->FindPropertyByName(FName(propertyName)));
		if (!property)
		{
			UE_LOG(LogGameDB, Error, TEXT("Failed to find property: %s"), *propertyName);
			continue;
		}

		void* data = property->ContainerPtrToValuePtr<void>(&myData);
		if (!data)
		{
			UE_LOG(LogGameDB, Error, TEXT("Failed to find data: %s"), *propertyName);
			continue;
		}

		FJsonObjectWrapper wrapper;
		if (!wrapper.JsonObjectFromString(json))
		{
			UE_LOG(LogGameDB, Error, TEXT("Failed to parse json: %s"), *json);
			continue;
		}
		
		if (!FJsonObjectConverter::JsonObjectToUStruct(wrapper.JsonObject.ToSharedRef(), property->Struct, data))
		{
			UE_LOG(LogGameDB, Error, TEXT("Failed to parse json"));
		}
	}
	
	myData.myInitialized = true; 
}

void UBlob::OnReadError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Error, TEXT("Blob read error: %s"), *anError.Message);
}

void UBlob::Set(const FBlobData& someData)
{
	if (!someData.myInitialized)
	{
		UE_LOG(LogGameDB, Error, TEXT("Blob not yet initialized"));
		return;
	}
	
	const auto client = GetClient();
	const auto session = GetSession();
	if (!client || !session)
	{
		UE_LOG(LogGameDB, Error, TEXT("GameDB not yet connected on blob init")); 
		return;
	}

	FOnStorageObjectAcks success;
	success.AddUniqueDynamic(this, &UBlob::OnWriteSuccess);
	FOnError error;
	error.AddUniqueDynamic(this, &UBlob::OnWriteError);
	
	TArray<FNakamaStoreObjectWrite> objects;
	for (TFieldIterator<UProperty> itr(myData.StaticStruct()); itr; ++itr)
	{
		const auto property = Cast<UStructProperty>(*itr);
		if (!property)
			continue; // Not a struct
		
		FString variableName = property->GetName();
		void const* currData = property->ContainerPtrToValuePtr<void>(&myData);
		void const* newData = property->ContainerPtrToValuePtr<void>(&someData);
		if (!currData || !newData)
		{
			UE_LOG(LogGameDB, Error, TEXT("Failed to get property data: %s"), *variableName);
			continue; 
		}
			
		UE_LOG(LogGameDB, Log, TEXT("Blob set compare: %s"), *variableName); 
		if (property->Identical(currData, newData, 0)) // Maybe adjust these flags? 
		{
			UE_LOG(LogGameDB, Log, TEXT("Blob data equal: %s"), *variableName);
			continue;
		}

		FString json;
		const bool result = FJsonObjectConverter::UStructToJsonObjectString(
			property->Struct,
			newData,
			json);
			
		if (!result || json.IsEmpty())
		{
			UE_LOG(LogGameDB, Error, TEXT("Failed to serialize blob data: %s"), *variableName);
			continue;
		}
			
		auto& obj = objects.Emplace_GetRef();
		obj.Collection = "blob";
		obj.Key = variableName; // property name
		obj.Value = json; // property json
		obj.PermissionRead = ENakamaStoragePermissionRead::OWNER_READ;
		obj.PermissionWrite = ENakamaStoragePermissionWrite::OWNER_WRITE;
	}

	if (objects.Num())
	{
		myData = someData; 
		client->WriteStorageObjects(session, objects, success, error);
	}
	else
	{
		UE_LOG(LogGameDB, Log, TEXT("Blob didn't write because nothing changed")); 
	}
}

void UBlob::OnWriteSuccess(const FNakamaStoreObjectAcks& someObjects)
{
	UE_LOG(LogGameDB, Log, TEXT("Blob write success"));
}

void UBlob::OnWriteError(const FNakamaError& anError)
{
	UE_LOG(LogGameDB, Error, TEXT("Blob write error: %s"), *anError.Message);
}
