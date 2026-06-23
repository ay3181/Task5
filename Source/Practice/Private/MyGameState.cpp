#include "MyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "MyGameInstance.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AMyGameState::AMyGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	
	CurrentLevelIndex = 0;
	MaxLevel = 3;

	WaveDuration = 60.0f;
	ItemToSpawn = 20;
	CurrentWaveIndex = 0;
	MaxWaveLevel = 3;
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();
	StartLevel();

	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this, &AMyGameState::UpdateHUD, 0.1f, true);
}

#pragma region Wave
void AMyGameState::StartWave()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowGameHUD();
			MyPlayerController->ShowWaveText();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentWaveIndex = MyGameInstance->CurrentWaveIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	SpawnItem();
	GetWorldTimerManager().SetTimer(
		SpawnTimer,
		this,
		&AMyGameState::SpawnItem,
		15.0f,
		true);

	GetWorldTimerManager().SetTimer(
		WaveTimer,
		this,
		&AMyGameState::EndWave,
		WaveDuration,
		false);
}

void AMyGameState::SpawnItem()
{

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor)
				{
					SpawnedItems.Add(SpawnedActor);
					if (SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
			}
		}
	}
}

void AMyGameState::DestroyAllItems()
{
	for (AActor* Item : SpawnedItems)
	{
		if (IsValid(Item))
		{
			Item->Destroy();
		}
	}
	SpawnedItems.Empty();
}

void AMyGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	GetWorldTimerManager().ClearTimer(WaveTimer);
	WaveDuration -= 15;
	ItemToSpawn += 5;

	DestroyAllItems();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			AddScore(Score);
			CurrentWaveIndex++;
			MyGameInstance->CurrentWaveIndex = CurrentWaveIndex;
		}
	}

	if (CurrentWaveIndex >= MaxWaveLevel)
	{
		EndLevel();
		return;
	}
	else
	{
		StartWave();
	}
}
#pragma endregion


#pragma region Level
void AMyGameState::StartLevel()
{
	WaveDuration = 60.0f;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex;
			MyGameInstance->CurrentWaveIndex = 0;
		}
	}
	StartWave();
}

void AMyGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	CurrentLevelIndex++;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			MyGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevel)
	{
		OnGameOver();
		return;
	}
	
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else {
		OnGameOver();
	}
}

void AMyGameState::OnGameOver()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	GetWorldTimerManager().ClearTimer(WaveTimer);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->SetPause(true);
			MyPlayerController->ShowMainMenu(true);
		}
	}
}
#pragma endregion


#pragma region Score
int32 AMyGameState::GetScore() const
{
	return Score;
}
void AMyGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			MyGameInstance->AddToScore(Amount);
		}
	}
}
#pragma endregion

void AMyGameState::OnCoinCollected()
{
	CollectedCoinCount++;
}

#pragma region UI
void AMyGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimer);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
						if (MyGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), MyGameInstance->TotalScore)));
						}
					}
				}
				if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrentLevelIndex+1)));
				}
				if (UTextBlock* WaveLevelText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveLevelText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrentWaveIndex + 1)));
				}
			}
		}
	}
}
#pragma endregion