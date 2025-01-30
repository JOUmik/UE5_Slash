// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FInputActionValue;
class ASlashCharacter;
/**
 * 
 */
class SLASH_API ICharacterState
{
public:
	ICharacterState();
	virtual ~ICharacterState();

	virtual void HandleInput(const FInputActionValue& Value) = 0;
	virtual void EnterState(ASlashCharacter* Character) = 0;
	virtual void ExitState(ASlashCharacter* Character) = 0;
	virtual void UpdateState(ASlashCharacter* Character, float DeltaTime) = 0;
};
