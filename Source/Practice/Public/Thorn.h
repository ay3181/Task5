#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Thorn.generated.h"

UCLASS()
class PRACTICE_API AThorn : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AThorn();

	int32 ThornDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thorn")
	FTimerHandle MoveTimer;

	void BeginPlay() override;

protected:
	virtual void ActivateItem(AActor* Activator) override;
	void RandomLocation();
};
