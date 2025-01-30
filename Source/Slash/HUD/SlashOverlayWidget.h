// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "SlashOverlayWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	bool StaminaRecover = false;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|ProgressBar")
	float DecreasePerSeconds = 0.18f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|ProgressBar")
	float RecoverySpeedPerSeconds = 0.35f;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void SetHealthBarPercent(float Percent) const;
	void SetStaminaBarPercent(float Percent) const;
	FORCEINLINE float GetStaminaBarPercent() const { return StaminaProgressBar->GetPercent(); }
	void SetHealthBarDelay();
	void SetStaminaBarDelay();
	void SetGolds(int32 GoldsNum) const;
	void AddGolds(int32 GoldsNum) const;
	void SetSouls(int32 SoulsNum) const;
	void AddSouls(int32 SoulsNUm) const;
	FORCEINLINE void StartRecovery() { StaminaRecover = true; }
	FORCEINLINE void StopRecovery() { StaminaRecover = false; }

private:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthProgressBar_L;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthProgressBar;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* StaminaProgressBar_L;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* StaminaProgressBar;
	UPROPERTY(meta=(BindWidget))
	UImage* HealthBarOverlay;
	UPROPERTY(meta=(BindWidget))
	UImage* OverlayIcon;
	UPROPERTY(meta=(BindWidget))
	UImage* CoinsIcon;
	UPROPERTY(meta=(BindWidget))
	UImage* SoulsIcon;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* GoldCount;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SoulCount;

	bool HealthBarDelay = false;
	bool StaminaBarDelay = false;
};
