// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetPointWidget.generated.h"

class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class SLASH_API UTargetPointWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;
	UPROPERTY(meta=(BindWidget))
	UImage* TargetPoint;
};
