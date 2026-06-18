#include "BigCoinItem.h"

ABigCoinItem::ABigCoinItem()
{
	CoinValue = 50;
	ItemName = "BigCoin";
}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}