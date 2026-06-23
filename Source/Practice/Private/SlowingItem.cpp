#include "SlowingItem.h"
#include "MyCharacter.h"

ASlowingItem::ASlowingItem()
{
	NerfTime = 8.0f;
	OriginSpeed = 300;
	OriginSprintSpeed = 450;
	ItemName = "SlowingItem";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(Activator))
		{
			OriginSpeed = MyCharacter->NormalSpeed;
			OriginSprintSpeed = MyCharacter->SprintSpeed;

			MyCharacter->NormalSpeed = OriginSpeed / 2;
			MyCharacter->SprintSpeed = OriginSprintSpeed / 2;

			//에이타니 도움받음
			FTimerDelegate SpeedBackDelegate;
			SpeedBackDelegate.BindLambda([this, MyCharacter]()
				{
					if (IsValid(MyCharacter))
					{
						MyCharacter->NormalSpeed = OriginSpeed;
						MyCharacter->SprintSpeed = OriginSprintSpeed;
					}
				});

			GetWorld()->GetTimerManager().SetTimer(
				SpeedBack,
				SpeedBackDelegate,
				NerfTime,
				false
			);
		}
		DestroyItem();
	}
}