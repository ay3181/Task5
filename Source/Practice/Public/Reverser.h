#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Reverser.generated.h"

UCLASS()
class PRACTICE_API AReverser : public ABaseItem
{
	GENERATED_BODY()

	AReverser();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reverser")
	FTimerHandle BackTimer;
	UFUNCTION(BlueprintCallable)
	virtual void ActivateItem(AActor* Activator) override;
	
protected:
	float NerfTime;
};
