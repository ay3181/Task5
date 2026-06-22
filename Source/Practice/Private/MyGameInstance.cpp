#include "MyGameInstance.h"
#include "Engine/Engine.h"

UMyGameInstance::UMyGameInstance()
{
	TotalScore = 0;
	CurrentWaveIndex = 0;
	CurrentLevelIndex = 0;
}

void UMyGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}