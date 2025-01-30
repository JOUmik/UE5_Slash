// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickUpInterface.generated.h"

class ATreasure;
class ASoul;
class AItem;

UINTERFACE(MinimalAPI)
class UPickUpInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IPickUpInterface
{
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(AItem* Item) = 0;
	virtual void AddSouls(ASoul* Soul) = 0;
	virtual void AddGolds(ATreasure* Treasure) = 0;
};
