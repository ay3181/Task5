#include "Thorn.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"

AThorn::AThorn()
{
	ItemName = "Thorn";
	ThornDamage = 10;
}

void AThorn::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		MoveTimer,
		this,
		&AThorn::RandomLocation,
		6.0f,
		true
	);
}

void AThorn::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(Activator, ThornDamage, nullptr, this, UDamageType::StaticClass());
	}
}

void AThorn::RandomLocation()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
	SetActorLocation(SpawnVolume->GetRandomPointInVolume());
}
