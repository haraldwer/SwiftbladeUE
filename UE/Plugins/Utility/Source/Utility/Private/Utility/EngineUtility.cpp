#include "Utility/EngineUtility.h"

bool UEngineUtility::IsShipping()
{
#ifdef UE_BUILD_SHIPPING
	return true;
#endif
	return false;
}
