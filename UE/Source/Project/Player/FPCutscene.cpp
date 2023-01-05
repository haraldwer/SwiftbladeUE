#include "FPCutscene.h"

#include "Animation/FPAnimatorNew.h"
#include "Animation/States/FPAnimationStateShowSword.h"

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
