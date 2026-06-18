#include "SmallCoinItem.h"

ASmallCoinItem::ASmallCoinItem()
{
	CoinValue = 10;
	ItemName = "SmallCoin";
}

void ASmallCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}