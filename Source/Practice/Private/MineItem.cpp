#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30;
	ItemName = "Mine";

	ExplosionRange = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRange"));
	ExplosionRange->SetupAttachment(RootComponent);
	ExplosionRange->InitSphereRadius(ExplosionRadius);
	ExplosionRange->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AMineItem::ActivateItem(AActor* Activator)
{
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);
}

void AMineItem::Explode()
{
	TArray<AActor*> OverlapActors;
	ExplosionRange->GetOverlappingActors(OverlapActors);
	for (AActor* Actor : OverlapActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, nullptr, this, UDamageType::StaticClass());
		}
	}
	DestroyItem();
}