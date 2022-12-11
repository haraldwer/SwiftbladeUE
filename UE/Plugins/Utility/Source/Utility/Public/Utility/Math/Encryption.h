#pragma once

#include "CoreMinimal.h"

inline TCHAR OffsetChar(const FString& aHash, const int32 anIndex, const TCHAR aChar, const bool aRead)
{
	// TODO: Complicate the offset calculations
	const int32 hashIndex = anIndex % aHash.Len();
	const TCHAR c = aHash[hashIndex];
	const int32 offset = static_cast<int32>(c);
	
	if (aRead)
	{
		return aChar - offset;
	}
	else
	{
		return aChar + offset;		
	} 
}

inline FString Encrypt(const FString& aString)
{
	return aString; 
	
	// generate hash
	const auto charArr = aString.GetCharArray();
	FBlake3Hash hashData = FBlake3::HashBuffer(charArr.GetData(), sizeof(wchar_t) * charArr.Num());
	const uint8* bytes = hashData.GetBytes();
	const FString hash = FString::FromBlob(bytes, 32);
	
	// use hash to transform string	
	FString result = aString;
	for (int32 index = 0; index < result.Len(); index++)
		result[index] = OffsetChar(hash, index, result[index], false);

	// store hash in result, currently at the beginning
	// TODO: Store with an offset
	result.InsertAt(0, hash);

	return result; 
}

inline FString Decrypt(const FString& aString)
{
	return aString; 
	
	// hash stored in result
	const FString hash = aString.Left(32);

	// use hash to transform string
	FString result = aString.RightChop(32);
	for (int32 index = 0; index < result.Len(); index++)
		result[index] = OffsetChar(hash, index, result[index], true);
	
	return result; 
}
