#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class PRACTICE_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	USphereComponent* ExplosionRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle ExplosionTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;
	void Explode();
};
