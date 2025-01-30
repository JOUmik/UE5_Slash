// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBarWidget;
/**
 * 
 */
UCLASS()
class SLASH_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetHealthPercent(const float Percent);
	void OwnerDead();
	void SetHealthBarDelay();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float DisplayTimeLeft = 0.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|HealthBarComponent")
	float MaxDisplayTime = 3.f;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	UHealthBarWidget* HealthBarWidget;

	UPROPERTY(EditAnywhere, Category = "CPP Settings|HealthBarComponent")
	float DecreasePerSeconds = 0.15f;
	bool bIsAlive = true;
	bool bHealthBarDelay = false;
};
