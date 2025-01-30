// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TargetPointComponent.generated.h"

class UTargetPointWidget;
/**
 * 
 */
UCLASS()
class SLASH_API UTargetPointComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void OwnerDead() const;
	void ShowTargetPoint() const; 
	void HideTargetPoint() const;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	UTargetPointWidget* TargetPointWidget;
};
