#include "FPCutscene.h"

#include "Project/UI/Prompts/PromptManager.h"
#include "Project/Utility/MainSingelton.h"

UFPCutscene::UFPCutscene()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFPCutscene::StartCutscene(const EFPCutsceneType aType)
{
	switch (aType)
	{
	case INTRO:
		{
			// TODO: Play "show sword" animation
			GetAnimator().SetState<UFPAnimationStateShowSword>();
			
		}
		break;
	case ENTER_ARENA:
		break;
	case EXIT_ARENA:
		break;
	case OUTRO:
		break;
	default: ;
	}

	if (myOnCutsceneStarted.IsBound())
		myOnCutsceneStarted.Broadcast(aType);
}
