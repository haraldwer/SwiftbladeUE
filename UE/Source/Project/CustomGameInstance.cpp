
#include "CustomGameInstance.h"

void UCustomGameInstance::AddPersistentObject(UObject* anObject)
{
	CHECK_RETURN_LOG(!anObject, "Invalid ptr"); 
	myPersistentObjects.Add(anObject);
	anObject->AddToRoot();
}

void UCustomGameInstance::RemovePersistentObject(UObject* anObject)
{
	CHECK_RETURN_LOG(!anObject, "Invalid ptr");
	myPersistentObjects.Remove(anObject);
	anObject->RemoveFromRoot();
}
