#pragma once

#include "Engine/Engine.h"

#define LOG_CONSOLE(text) \
UE_LOG(LogTemp, Display, TEXT("%s"), *FString(FString(text) + " | " + FString(__FUNCTION__) + " line " + FString::FromInt(__LINE__)));
#define LOG(text) \
{ GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString(text)); \
LOG_CONSOLE(text) };

#define CHECK_ASSERT(condition, text) \
check(!(condition) && text);

#define CHECK_RETURN(condition, ...) \
if (condition) { return __VA_ARGS__; }

#define CHECK_RETURN_LOG(condition, text, ...) \
if (condition) { LOG(text); return __VA_ARGS__; }

#define CHECK_BREAK(condition) \
if (condition) { break; }

#define CHECK_BREAK_LOG(condition, text) \
if (condition) { LOG(text); break; }

#define CHECK_CONTINUE(condition) \
if (condition) { continue; }

#define CHECK_CONTINUE_LOG(condition, text) \
if (condition) { LOG(text); continue; }

#define DECLARE_CHECK_RETURN(name, func, ...) \
auto name = func; \
CHECK_RETURN(!name, FString(#name) + FString(" was nullptr"), __VA_ARGS__)