#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AMineItem::AMineItem()
{
	ExplosionDelay = 2.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30;
	ItemName = "Mine";
	bHasExploded = false;

	ExplosionRange = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRange"));
	ExplosionRange->SetupAttachment(RootComponent);
	ExplosionRange->InitSphereRadius(ExplosionRadius);
	ExplosionRange->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;
	Super::ActivateItem(Activator);

	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);
	bHasExploded = true;
}

void AMineItem::Explode()
{
	if (!GetWorld() || !IsValid(ExplosionRange))
	{
		return;
	}

	UParticleSystemComponent* Particle = nullptr ;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation(), false);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation( GetWorld(), ExplosionSound, GetActorLocation(), GetActorRotation() );
	}

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
	
	if (IsValid(Particle))
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				DestroyParticleTimerHandle,
				[WeakParticle]()
				{
					if (WeakParticle.IsValid())
					{
						WeakParticle->DestroyComponent();
					}
				},
				2.0f,
				false
			);
		}
	}
}