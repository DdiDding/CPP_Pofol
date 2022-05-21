#include "Util/CLog.h"
DEFINE_LOG_CATEGORY(MyLog);

void CLog::Log(int data, FString introduction)
{
	introduction += FString::FromInt(data);
	UE_LOG(MyLog, Warning, TEXT("%s"), *introduction);
}

void CLog::Log(float data, FString introduction)
{
	introduction += FString::SanitizeFloat(data);
	UE_LOG(MyLog, Warning, TEXT("%s"), *introduction);
}

void CLog::Log(FVector2D data, FString introduction)
{
	introduction += data.ToString();
	UE_LOG(MyLog, Warning, TEXT("%s"), *introduction);
}

void CLog::Log(FVector data, FString introduction)
{
	introduction += data.ToString();
	UE_LOG(MyLog, Warning, TEXT("%s"), *introduction);
}

void CLog::Log(FRotator data, FString introduction)
{
	introduction += data.ToString();
	UE_LOG(MyLog, Warning, TEXT("%s"), *introduction);
}

void CLog::Log(FString data, FString introduction)
{
	introduction += data;
	UE_LOG(MyLog, Warning, TEXT("%s"), *introduction);
}

void CLog::ScreenLog(int data, float time, FColor color, FString introduction)
{
	introduction += FString::FromInt(data);
	GEngine->AddOnScreenDebugMessage(-1, time, color, *introduction);
}

void CLog::ScreenLog(float data, float time, FColor color, FString introduction)
{
	introduction += FString::SanitizeFloat(data);
	GEngine->AddOnScreenDebugMessage(-1, time, color, *introduction);
}

void CLog::ScreenLog(FVector data, float time, FColor color, FString introduction)
{
	introduction += data.ToString();
	GEngine->AddOnScreenDebugMessage(-1, time, color, *introduction);
}

void CLog::ScreenLog(FRotator data, float time, FColor color, FString introduction)
{
	introduction += data.ToString();
	GEngine->AddOnScreenDebugMessage(-1, time, color, *introduction);
}

void CLog::ScreenLog(FString introduction, float time, FColor color)
{
	GEngine->AddOnScreenDebugMessage(-1, time, color, *introduction);
}
