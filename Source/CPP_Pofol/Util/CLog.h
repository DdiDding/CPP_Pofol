#pragma once
#include "CoreMinimal.h"
#include <cstdarg>

/**
 * �α׸� ���� ����ϱ����� Ŭ����
 */

DECLARE_LOG_CATEGORY_EXTERN(MyLog, Warning, All)

typedef char* va_list;

class CPP_POFOL_API CLog
{
public:

	static void Log(int data,	FString introduction = FString(TEXT(" ")));
	static void Log(float data, FString introduction = FString(TEXT(" ")));
	static void Log(FVector2D data, FString introduction = FString(TEXT(" ")));
	static void Log(FVector data, FString introduction = FString(TEXT(" ")));
	static void Log(FRotator data, FString introduction = FString(TEXT(" ")));
	static void Log(FString data, FString introduction = FString(TEXT(" ")));

	static void ScreenLog(int data, float time = 0.0f, FColor color = FColor::Red, FString introduction = FString(TEXT(" ")));
	static void ScreenLog(float data, float time = 0.0f, FColor color = FColor::Red, FString introduction = FString(TEXT(" ")));
	static void ScreenLog(FVector data, float time = 0.0f, FColor color = FColor::Red, FString introduction = FString(TEXT(" ")));
	static void ScreenLog(FRotator data, float time = 0.0f, FColor color = FColor::Red, FString introduction = FString(TEXT(" ")));
	static void ScreenLog(FString introduction, float time = 0.0f, FColor color = FColor::Red);

	//TODO: debug arrow, line  ���⼭ ó���ϱ�
};
