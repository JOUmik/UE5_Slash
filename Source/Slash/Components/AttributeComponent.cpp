// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UAttributeComponent::SetStaminaByPercent(float Percent)
{
	Percent = FMath::Clamp(Percent, 0.f, 1.f);
	CurStamina = MaxStamina * Percent;
}

void UAttributeComponent::ReceiveDamage(const float Damage)
{
	CurHealth = FMath::Clamp(CurHealth - Damage, 0, MaxHealth);
}

void UAttributeComponent::CostStamina(const float Cost)
{
	CurStamina = FMath::Clamp(CurStamina - Cost, 0, MaxStamina);
}

