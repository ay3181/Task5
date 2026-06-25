#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

UCLASS()
class PRACTICE_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMyGameState();

#pragma region Score
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
#pragma endregion


#pragma region Coin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	void OnCoinCollected();
#pragma endregion


#pragma region Wave
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentWaveIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxWaveLevel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float WaveDuration;
	UPROPERTY()
	TArray<AActor*> SpawnedItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<AActor> ThornClass;

	FTimerHandle WaveTimer;
	int32 ItemToSpawn;
	FTimerHandle SpawnTimer;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWave();
	void SpawnItem();
	void SpawnThorn();
	void DestroyAllItems();
	void EndWave();
#pragma endregion


#pragma region Level
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevel;
	

	void StartLevel();
	void EndLevel();
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
#pragma endregion

#pragma region UI
	FTimerHandle HUDUpdateTimerHandle;
	void UpdateHUD();
#pragma endregion


	virtual void BeginPlay() override;
};
