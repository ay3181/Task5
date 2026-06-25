#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class PRACTICE_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()
public:
	ASlowingItem();

	FTimerHandle SpeedBack;
	virtual void ActivateItem(AActor* Activator) override;

protected:
	float NerfTime;
};
