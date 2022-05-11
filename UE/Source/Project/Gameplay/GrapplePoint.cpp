#include "GrapplePoint.h"

AGrapplePoint::AGrapplePoint()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void AGrapplePoint::SetFocus(bool aFocused)
{
	CHECK_RETURN(myFocused == aFocused);
	aFocused ? OnBeginFocus() : OnEndFocus();
	myFocused = aFocused;
}
