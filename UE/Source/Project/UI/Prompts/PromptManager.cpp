#include "PromptManager.h"

#include "PromptBase.h"
#include "Project/UI/Menus/MenuBase.h"

APromptManager::APromptManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APromptManager::BeginPlay()
{
	Super::BeginPlay();
	
}

UPromptBase* APromptManager::CreatePrompt(const EPromptType aPrompt)
{
	CHECK_RETURN_LOG(aPrompt == EPromptType::UNKNOWN, "Unknown prompt type", nullptr);
	CHECK_RETURN_LOG(IsPromptOpen(aPrompt), "Prompt already open", nullptr);
	const auto instance = GetPromptInstance(aPrompt);
	CHECK_RETURN_LOG(!instance, "Could not create or find prompt instance", nullptr);
	instance->OnCreate();
	instance->AddToCamera();
	myOpenPrompts.Add(instance); 
	return instance; 
}

void APromptManager::DestroyPrompt(const EPromptType aPrompt)
{
 	CHECK_RETURN(!myOpenPrompts.Num());
	for (int i = myOpenPrompts.Num() - 1; i >= 0; i--)
	{
		const auto prompt = myOpenPrompts[i];
		CHECK_CONTINUE(!prompt); 
		CHECK_CONTINUE(prompt->GetPromptType() != aPrompt);
		prompt->OnDestroy();
		prompt->RemoveFromCamera();
		myOpenPrompts.RemoveAt(i);
	}
}

bool APromptManager::IsPromptOpen(const EPromptType aPrompt) const
{
	for (const auto& it : myOpenPrompts)
		if (it->GetPromptType() == aPrompt)
			return true;
	return false;
}

bool APromptManager::IsAnyPromptOpen() const
{
	return myOpenPrompts.Num() > 0; 
}

UPromptBase* APromptManager::GetPromptInstance(const EPromptType aPrompt)
{
	for (const auto& prompt : myPrompts)
		if (prompt->GetPromptType() == aPrompt)
			return prompt;

	const auto blueprint = myPromptBlueprints.Find(aPrompt);
	CHECK_RETURN_LOG(!blueprint, "Unable to find prompt blueprint", nullptr);
	const auto world = GetWorld();
	CHECK_RETURN_LOG(!world, "World nullptr", nullptr);
	const auto prompt = CreateWidget(world, blueprint->Get());
	CHECK_RETURN_LOG(!prompt, "Failed to create prompt", nullptr);
	const auto promptBase = Cast<UPromptBase>(prompt);
	CHECK_RETURN_LOG(!promptBase, "Prompt does not inherit from UPromptBase", nullptr);
	myPrompts.Add(promptBase);
	promptBase->Init(aPrompt);
	return promptBase;
}

