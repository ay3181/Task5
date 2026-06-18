#include "MyGameInstance.h"
#include "Engine/Engine.h"

UMyGameInstance::UMyGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UMyGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Score: %d"),TotalScore));
}