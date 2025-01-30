// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlayWidget;
/**
 * 
 */
UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "CPP Settings")
	TSubclassOf<USlashOverlayWidget> SlashOverlayClass;

	UPROPERTY()
	USlashOverlayWidget* SlashOverlayWidget;

	
public: //Getters and Setters
	FORCEINLINE USlashOverlayWidget* GetSlashOverlayWidget() const { return SlashOverlayWidget; }
};
