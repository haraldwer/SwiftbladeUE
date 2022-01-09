#include "PromptBase.h"

#include "Project/Utility/MainSingelton.h"

void UPromptBase::Init(const EPromptType aPrompt)
{
	myPromptType = aPrompt;
}

void UPromptBase::Destroy()
{
	UMainSingelton::GetPromptManager().DestroyPrompt(GetPromptType());
}
