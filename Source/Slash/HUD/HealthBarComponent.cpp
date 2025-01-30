// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"

#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	HealthBarWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());

	HealthBarWidget->HealthBar->SetPercent(1.f);
	HealthBarWidget->HealthBar_L->SetPercent(1.f);
	//HealthBar is hidden by default, only when the character is damaged, show the HealthBar
	HealthBarWidget->SetRenderOpacity(0.f);
}

void UHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//if HealthBar is displaying, determine whether the display time has run out. If so, hide HealthBar
	if(DisplayTimeLeft > 0)
	{
		DisplayTimeLeft-=DeltaTime;
		if(DisplayTimeLeft <= 0)
		{
			if(HealthBarWidget)
			{
				HealthBarWidget->SetRenderOpacity(0.f);
			}
		}
	}

	if(bHealthBarDelay)
	{
		if(HealthBarWidget->HealthBar_L->GetPercent() > HealthBarWidget->HealthBar->GetPercent())
		{
			const float DecreaseNum = DecreasePerSeconds * DeltaTime;
			HealthBarWidget->HealthBar_L->SetPercent(HealthBarWidget->HealthBar_L->GetPercent() - DecreaseNum);
		}
		else
		{
			bHealthBarDelay = false;
			HealthBarWidget->HealthBar_L->SetPercent(HealthBarWidget->HealthBar->GetPercent());
		}
	}
}

void UHealthBarComponent::SetHealthPercent(const float Percent)
{
	if(HealthBarWidget && bIsAlive)
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
		//Show the HealthBar
		HealthBarWidget->SetRenderOpacity(1.0f);
		DisplayTimeLeft = MaxDisplayTime;
	}
}

void UHealthBarComponent::OwnerDead()
{
	HealthBarWidget->SetRenderOpacity(0.f);
	bIsAlive = false;
	DisplayTimeLeft = 0.f;
}

void UHealthBarComponent::SetHealthBarDelay()
{
	bHealthBarDelay = true;
}
