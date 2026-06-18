#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

ASpawnVolume::ASpawnVolume()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(SceneComponent);

	ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectItem = GetRandomItem()) //랜덤 아이템 행을 받아오면
	{
		if (UClass* ActualClass = SelectItem->ItemClass.Get()) //그 아이템 행에서 클래스를 가져옴
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) { return nullptr; }
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) { return nullptr; }

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandomValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f; //누적 확률

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandomValue<= AccumulateChance)
		{
			return Row;
		}
	}
	
	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawnBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnBox->GetComponentLocation();

	return BoxOrigin + FVector(FMath::FRandRange(-BoxExtent.X, BoxExtent.X), FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y), FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(), FRotator::ZeroRotator);
	return SpawnedActor;
}