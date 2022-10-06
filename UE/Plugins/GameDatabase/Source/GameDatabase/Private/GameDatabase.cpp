
#include "GameDatabase.h"

#define NANODBC_DISABLE_ASYNC 1
#define NANODBC_ENABLE_UNICODE 1
#define WIN32_LEAN_AND_MEAN

#include "unicode_utils.h"
#include "nanodbc.h"
#include <locale>
#include <codecvt>
#include <string>

IMPLEMENT_MODULE(FGameDatabase, GameDatabase);

DEFINE_LOG_CATEGORY(LogGameDB);

void FGameDatabase::StartupModule()
{
	IModuleInterface::StartupModule();
	Open();
}

void FGameDatabase::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

void FGameDatabase::Open()
{
	return;
	nanodbc::connection connection(convert(WideToNarrow(myConnectionString)), myTimeoutDuration);
	UE_LOG(LogGameDB, Display, TEXT("%s"), *NarrowToWide(convert(connection.driver_name())));

	// Direct execution
	{
		execute(connection, NANODBC_TEXT("insert into simple_test values (1, 'one');"));
		nanodbc::result results = execute(connection, NANODBC_TEXT("select * from simple_test;"));
	}

	// Accessing results by name, or column number
	{
		nanodbc::result results = execute(
			connection,
			NANODBC_TEXT("select a as first, b as second from simple_test where a = 1;"));
		results.next();
		auto const value = results.get<nanodbc::string>(1);	
	}
}

void FGameDatabase::Close()
{
}

void FGameDatabase::Upload(const FString& aKey)
{
}

void FGameDatabase::Query(const FString& aKey)
{
}

std::string FGameDatabase::WideToNarrow(const FString& aString)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(*aString);
}

FString FGameDatabase::NarrowToWide(const std::string& aString)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return FString(converter.from_bytes(aString).c_str());
}
