#include "PromptBase.h"

void UPromptBase::Init(const EPromptType aPrompt)
{
	myPromptType = aPrompt;
}

void UPromptBase::Destroy()
{
	UPromptSubsystem::Get().DestroyPrompt(GetPromptType());
}
