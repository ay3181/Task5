#include "SlowingItem.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "MyPlayerController.h"

ASlowingItem::ASlowingItem()
{
	NerfTime = 8.0f;
	ItemName = "SlowingItem";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	//람다식 쓰기가 어려워서 에이타니 도움받음
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(Activator))
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(MyCharacter->GetController()))
		{
			MyPlayerController->ShowSlowImage();
		}

		float OriginSpeed = MyCharacter->NormalSpeed;
		float OriginSprintSpeed = MyCharacter->SprintSpeed;

		MyCharacter->NormalSpeed = OriginSpeed / 2;
		MyCharacter->SprintSpeed = OriginSprintSpeed / 2;

		if (MyCharacter->GetCharacterMovement())
		{
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed /= 2.0f;
		}

		FTimerDelegate SpeedBackDelegate;
		SpeedBackDelegate.BindLambda([MyCharacter, OriginSpeed, OriginSprintSpeed]()
		{
			if (IsValid(MyCharacter))
			{
				MyCharacter->NormalSpeed = OriginSpeed;
				MyCharacter->SprintSpeed = OriginSprintSpeed;

				if (MyCharacter->GetCharacterMovement())
				{
					MyCharacter->GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
				}
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