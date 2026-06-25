#include "Reverser.h"
#include "MyCharacter.h"
#include "MyPlayerController.h"

AReverser::AReverser()
{
	NerfTime = 8.0f;
	ItemName = "ReverseItem";
}

void AReverser::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(Activator))
		{
			if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(MyCharacter->GetController()))
			{
				MyPlayerController->ShowReverseImage();
			}

			MyCharacter->IsMoveReverse = true;

			FTimerDelegate MoveBackDelegate;
			MoveBackDelegate.BindLambda([MyCharacter]()
				{
					if (IsValid(MyCharacter))
					{
						MyCharacter->IsMoveReverse = false;
					}
				});

			GetWorld()->GetTimerManager().SetTimer(
				BackTimer,
				MoveBackDelegate,
				NerfTime,
				false
			);
		}
		DestroyItem();
	}
}