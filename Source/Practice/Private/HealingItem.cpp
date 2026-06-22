#include "HealingItem.h"
#include "MyCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20;
	ItemName = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* MyPlayer = Cast<AMyCharacter>(Activator))
		{
			MyPlayer->AddHealth(HealAmount);
		}
		DestroyItem();
	}
}