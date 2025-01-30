// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TargetPointComponent.h"

#include "TargetPointWidget.h"

void UTargetPointComponent::OwnerDead() const
{
	TargetPointWidget->SetRenderOpacity(0.f);
}

void UTargetPointComponent::ShowTargetPoint() const
{
	TargetPointWidget->SetRenderOpacity(1.f);
}

void UTargetPointComponent::HideTargetPoint() const
{
	TargetPointWidget->SetRenderOpacity(0.f);
}

void UTargetPointComponent::BeginPlay()
{
	Super::BeginPlay();

	TargetPointWidget = Cast<UTargetPointWidget>(GetUserWidgetObject());
	
	TargetPointWidget->SetRenderOpacity(0.f);
}
