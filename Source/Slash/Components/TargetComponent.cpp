// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargetComponent.h"

#include "Characters/Player/SlashAnimInstance.h"
#include "Characters/Player/SlashCharacter.h"
#include "Characters/Enemies/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTargetComponent::UTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = Cast<ASlashCharacter>(GetOwner());
	if(MyCharacter)
	{
		//Save the current rotation mode
		bUseControllerRotationYaw = MyCharacter->bUseControllerRotationYaw;
		bOrientRotationToMovement = MyCharacter->GetCharacterMovement()->bOrientRotationToMovement;
		bUseControllerDesiredRotation = MyCharacter->GetCharacterMovement()->bUseControllerDesiredRotation;
		OriginalMeshRotation = MyCharacter->GetMesh()->GetRelativeRotation();
		OriginalRotationRate = MyCharacter->GetCharacterMovement()->RotationRate;

		FreeModeMaxSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed;
	}
}

void UTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	KeepFocusOnTarget();
}

void UTargetComponent::FocusingModeRotation()
{
	//Change rotation mode when correctly lock on an enemy
	if(MyCharacter)
	{
		if(USlashAnimInstance* SlashAnimInstance = Cast<USlashAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance()))
		{
			//This is for the correct Animation state
			SlashAnimInstance->bIsFocusing = true;
		}

		//Change rotation as lock on the enemy, so player can move right in the targeting mode
		MyCharacter->bUseControllerRotationYaw = true;
		MyCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		MyCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		MyCharacter->GetCharacterMovement()->RotationRate = RotationRate;
		
		MyCharacter->SpringComp->bEnableCameraLag = true;
		MyCharacter->SpringComp->bEnableCameraRotationLag = true;
		MyCharacter->SpringComp->CameraLagSpeed = 24.f;
		MyCharacter->SpringComp->CameraRotationLagSpeed = 12.f;
		
		//do not need to control the camera anymore
		/*if(AController* Controller = MyCharacter->GetController())
		{
			Controller->SetIgnoreLookInput(true);
		}*/
		
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = FocusingModeMaxSpeed;
	}
}

void UTargetComponent::FreeModeRotation()
{
	if(MyCharacter)
	{
		if(USlashAnimInstance* SlashAnimInstance = Cast<USlashAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance()))
		{
			//This is for the correct Animation state
			SlashAnimInstance->bIsFocusing = false;
		}

		MyCharacter->bUseControllerRotationYaw = bUseControllerRotationYaw;
		MyCharacter->GetCharacterMovement()->bOrientRotationToMovement = bOrientRotationToMovement;
		MyCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
		MyCharacter->GetCharacterMovement()->RotationRate = OriginalRotationRate;

		MyCharacter->SpringComp->bEnableCameraLag = false;
		MyCharacter->SpringComp->bEnableCameraRotationLag = false;

		/*if(AController* Controller = MyCharacter->GetController())
		{
			Controller->SetIgnoreLookInput(false);
		}*/

		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = FreeModeMaxSpeed;
	}
}

//Target on the enemy
void UTargetComponent::FocusTarget(AEnemy* TargetEnemy)
{
	if(bIsFocusing) return;
	TargetEnemy->Select(); //Add Widget to Target Enemy
	FocusingModeRotation();
	bIsFocusing = true;
	FocusedActor = TargetEnemy;
}

//Cancel the target
void UTargetComponent::CancelFocusTarget()
{
	if(!bIsFocusing) return;
	FreeModeRotation();
	bIsFocusing = false;
	if(FocusedActor)
	{
		FocusedActor->CancelSelect(); //Remove Widget to Target Enemy
	}

	FocusedActor = nullptr;
}

void UTargetComponent::FindOrCancelTarget()
{
	//If already has focused target, cancel the lock and back to free mode
	if(bIsFocusing)
	{
		CancelFocusTarget();
		return;
	}

	if(!MyCharacter) return;
	
	//Choose the collision channel
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(TraceCollisionChannel);

	const FVector CameraLocation = MyCharacter->GetCameraLocation();
	const FRotator CameraRotation = MyCharacter->GetViewRotation();

	FVector End = CameraLocation + (CameraRotation.Vector() * TraceDistance);

	TArray<FHitResult> Hits;

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	bool bBlockHit = GetWorld()->SweepMultiByObjectType(Hits, CameraLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	
	for(FHitResult Hit:Hits)
	{
		if(AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor()))
		{
			if(HitEnemy->GetCurrentState() != EEnemyState::EES_Dead)
			{
				FocusTarget(HitEnemy);
				break;
			}
		}
	}

}

void UTargetComponent::DodgeCameraLagOpen()
{
	MyCharacter->SpringComp->bEnableCameraLag = true;
	MyCharacter->SpringComp->bEnableCameraRotationLag = true;
	MyCharacter->SpringComp->CameraRotationLagSpeed = 1.f;
}

void UTargetComponent::DodgeCameraLagClose()
{
	MyCharacter->SpringComp->CameraRotationLagSpeed = 1.f;
}

void UTargetComponent::KeepFocusOnTarget()
{
	if(!FocusedActor|| !MyCharacter) return;
	//If enemy was dead, clear target and return
	if(FocusedActor->GetCurrentState() == EEnemyState::EES_Dead)
	{
		CancelFocusTarget();
		return;
	}

	if(MyCharacter->GetActionState() != EActionState::EAS_Dodging)
	{
		if(MyCharacter->GetMesh()->GetRelativeRotation() != OriginalMeshRotation)
		{
			const FRotator CurRotation = FMath::RInterpConstantTo(MyCharacter->GetMesh()->GetRelativeRotation(), OriginalMeshRotation, GetWorld()->GetDeltaSeconds(), 1000.f);
			MyCharacter->GetMesh()->SetRelativeRotation(CurRotation);
		}
		if(MyCharacter->SpringComp->CameraRotationLagSpeed != 12.f)
		{
			const float InterpValue = FMath::FInterpConstantTo(MyCharacter->SpringComp->CameraRotationLagSpeed, 12.f, GetWorld()->GetDeltaSeconds(), 9.f);
			MyCharacter->SpringComp->CameraRotationLagSpeed = InterpValue;
		}
		
	}

	//Keep looking at the target

	//Find TargetRotation From Actor to Enemy;
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(
		MyCharacter->GetActorLocation(),
		FocusedActor->GetActorLocation());

	//do not need roll
	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = MyCharacter->GetController()->GetControlRotation().Pitch;

	//lerp to target
	const FRotator NewRotation = UKismetMathLibrary::RInterpTo_Constant(
		MyCharacter->GetControlRotation(),
		TargetRotation,
		GetWorld()->GetDeltaSeconds(),
		500.f);

	MyCharacter->GetController()->SetControlRotation(NewRotation);
}
