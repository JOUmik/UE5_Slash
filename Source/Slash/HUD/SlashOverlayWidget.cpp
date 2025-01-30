// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlayWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(HealthBarDelay)
	{
		if(HealthProgressBar_L->GetPercent() > HealthProgressBar->GetPercent())
		{
			const float DecreaseNum = DecreasePerSeconds * InDeltaTime;
			HealthProgressBar_L->SetPercent(HealthProgressBar_L->GetPercent() - DecreaseNum);
		}
		else
		{
			HealthBarDelay = false;
			HealthProgressBar_L->SetPercent(HealthProgressBar->GetPercent());
		}
	}

	if(StaminaBarDelay)
	{
		if(StaminaProgressBar_L->GetPercent() > StaminaProgressBar->GetPercent())
		{
			const float DecreaseNum = DecreasePerSeconds * InDeltaTime;
			StaminaProgressBar_L->SetPercent(StaminaProgressBar_L->GetPercent() - DecreaseNum);
		}
		else
		{
			StaminaBarDelay = false;
			StaminaProgressBar_L->SetPercent(StaminaProgressBar->GetPercent());
		}
	}

	if(StaminaRecover)
	{
		const float RecoveryNum = RecoverySpeedPerSeconds * InDeltaTime;
		StaminaProgressBar->SetPercent(StaminaProgressBar->GetPercent() + RecoveryNum);
		if(StaminaProgressBar->GetPercent() >= 1.f)
		{
			StaminaProgressBar->SetPercent(1.f);
			StaminaRecover = false;
		}

		if(StaminaProgressBar_L->GetPercent() <= StaminaProgressBar->GetPercent())
		{
			StaminaProgressBar_L->SetPercent(StaminaProgressBar->GetPercent());
		}
	}
}

void USlashOverlayWidget::SetHealthBarPercent(float Percent) const
{
	Percent = FMath::Clamp(Percent, 0.f, 1.f);
	
	if(HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlayWidget::SetStaminaBarPercent(float Percent) const
{
	Percent = FMath::Clamp(Percent, 0.f, 1.f);
	if(StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlayWidget::SetHealthBarDelay()
{
	HealthBarDelay = true;
}

void USlashOverlayWidget::SetStaminaBarDelay()
{
	StaminaBarDelay = true;
}

void USlashOverlayWidget::SetGolds(int32 GoldsNum) const
{
	if(GoldCount)
	{
		GoldCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), GoldsNum)));
	}
}

void USlashOverlayWidget::AddGolds(int32 GoldsNum) const
{
	if(GoldCount)
	{
		int32 DisplayNum = FCString::Atoi(*GoldCount->GetText().ToString());
		DisplayNum += GoldsNum;
		GoldCount->SetText(FText::FromString(FString::FromInt(DisplayNum)));
	}
	
}

void USlashOverlayWidget::SetSouls(int32 SoulsNum) const
{
	if(SoulCount)
	{
		SoulCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), SoulsNum)));
	}
}

void USlashOverlayWidget::AddSouls(int32 SoulsNUm) const
{
	if(SoulCount)
	{
		int32 DisplayNum = FCString::Atoi(*SoulCount->GetText().ToString());
		DisplayNum += SoulsNUm;
		SoulCount->SetText(FText::FromString(FString::FromInt(DisplayNum)));
	}
}
