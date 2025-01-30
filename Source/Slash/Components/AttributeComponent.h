// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	 * Health Functions
	 */
	void SetStaminaByPercent(float Percent);
	void ReceiveDamage(const float Damage);
	void CostStamina(const float Cost);
	FORCEINLINE void AddSouls(const int32 SoulsNum){ Souls += SoulsNum; }
	FORCEINLINE void AddGolds(const int32 GoldsNum){ Golds += GoldsNum; }
	FORCEINLINE float GetHealthPercent() const { return CurHealth/MaxHealth; }
	FORCEINLINE float GetStaminaPercent() const { return CurStamina/MaxStamina; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetGolds() const { return Golds; }
	FORCEINLINE bool IsAlive() const { return CurHealth > 0.f; }
	FORCEINLINE bool CanUseStamina(const float Cost) const { return CurStamina >=Cost; } 
protected:
	virtual void BeginPlay() override;

private: 
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Actor Attributes")
	float CurHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Actor Attributes")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Actor Attributes")
	float CurStamina = 200.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Actor Attributes")
	float MaxStamina = 200.f;
	UPROPERTY(VisibleAnywhere, Category = "CPP Settings|Actor Attributes")
	int32 Souls = 0;
	UPROPERTY(VisibleAnywhere, Category = "CPP Settings|Actor Attributes")
	int32 Golds = 0;

	
public:
	/*
	 * Getters and Setters
	 */
};
