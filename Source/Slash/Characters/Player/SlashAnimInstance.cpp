// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/SlashAnimInstance.h"
#include "Characters/Player/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if(SlashCharacter)
	{
		CharacterMovementComp = SlashCharacter->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(CharacterMovementComp)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovementComp->Velocity);
		bIsFalling = CharacterMovementComp->IsFalling();
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
	}
}
