// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "SlashCharacter.h"
#include "Animation/AnimInstance.h"
#include "SlashAnimInstance.generated.h"

enum class ECharacterState : uint8;
class UCharacterMovementComponent;
class ASlashCharacter;
/**
 * 
 */
UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	ASlashCharacter* SlashCharacter;
	UPROPERTY(BlueprintReadOnly, Category="CPP Settings|Movement")
	UCharacterMovementComponent* CharacterMovementComp;
	UPROPERTY(BlueprintReadOnly, Category="CPP Settings|Movement")
	float GroundSpeed = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="CPP Settings|Movement")
	bool bIsFalling = false;
	UPROPERTY(BlueprintReadOnly, Category="CPP Settings|Movement")
	bool bIsFocusing = false;
	UPROPERTY(BlueprintReadOnly, Category="CPP Settings|Movement")
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadOnly, Category="CPP Settings|Movement")
	EActionState ActionState = EActionState::EAS_Unoccupied;
};
