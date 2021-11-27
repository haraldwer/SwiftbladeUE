// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineUtility.h"

bool UEngineUtility::IsShipping()
{
#ifdef UE_BUILD_SHIPPING
	return true;
#endif
	return false;
}