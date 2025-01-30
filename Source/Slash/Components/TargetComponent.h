// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetComponent.generated.h"


class ASlashCharacter;
class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** Change from free mode to focusing mode */
	void FocusingModeRotation();
	/** Change from focusing mode to free mode */
	void FreeModeRotation();
	/** Target on the enemy */
	void FocusTarget(AEnemy* TargetEnemy);
	/** Cancel the target */
	void CancelFocusTarget();
	UFUNCTION(BlueprintCallable)
	void FindOrCancelTarget();

	void DodgeCameraLagOpen();
	void DodgeCameraLagClose();

	bool bIsFocusing = false;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ASlashCharacter* MyCharacter;
	
	bool bUseControllerRotationYaw;
	bool bOrientRotationToMovement;
	bool bUseControllerDesiredRotation;
	FRotator OriginalMeshRotation;
	FRotator OriginalRotationRate;

	float FreeModeMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Combat Properties")
	float FocusingModeMaxSpeed = 150.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Combat Properties")
	float TraceRadius = 50.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Combat Properties")
	float TraceDistance = 2000.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Combat Properties")
	FRotator RotationRate = FRotator(0, 20.f, 0);
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Combat Properties")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECC_Pawn;

	UPROPERTY()
	AEnemy* FocusedActor;

	void KeepFocusOnTarget();
};
