// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class ASoul;
class UTargetPointComponent;
class UPawnSensingComponent;
enum class EPatrolMethod : uint8;
enum class EEnemyState : uint8;
class AAIController;
enum class EDeathPose : uint8;
class UHealthBarComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override; /*<AActor>*/
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override; /*<AActor>*/

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /*<IHitInterface>*/

	//TargetLock Functions
	void Select();
	void CancelSelect();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EEnemyState GetCurrentState() const { return CurrentState; }
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

protected:
	virtual void BeginPlay() override;

	/* Behaviours */
	UFUNCTION()
	void OnSeePawn(APawn* Pawn);
	void MoveToTarget(AActor* Target);
	void LostInterest();
	void StartPatrolling();
	void ChaseTarget();
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void Die_Implementation() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(const float DamageAmount) const override;
	bool IsPatrolling() const;
	bool IsChasing() const;
	bool IsAttacking() const;
	bool IsEngaged() const;
	bool IsDead() const;
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool InTargetRange(AActor* TargetActor, const double Radius);
	/* _Behaviours */
	
	/* Play Montage Functions */
	virtual int32 PlayDeathMontage() override;
	/* _Play Montage Functions */

	
	/* Components */
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTargetPointComponent* TargetPointComp;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComp;
	/* _Components */

private:
	/* ENUM */
	void ChangeCurrentState(EEnemyState NewState);
	EEnemyState CurrentState;
	EDeathPose DeathPose;
	/* _ENUM */

	UPROPERTY(EditAnywhere, Category = "CPP Settings|Weapon")
	TSubclassOf<AWeapon> WeaponClass;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Soul")
	TSubclassOf<ASoul> SoulClass;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Soul")
	int32 SoulValue = 50.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Soul")
	FVector SpawnSoulOffset = FVector(0, 0, 60.f);
	
	/* Navigation Begin */
	void UpdateCombat();
	void UpdatePatrol();
	float UpdateCombatWalkSpeed() const;
	void Patrol_Circle();
	void Patrol_BackAndForth();
	void Patrol_OneWay();
	void Patrol_Random();
	
	bool bIsReverse_BackAndForth = false;
	bool bIsIdle = false;
	UPROPERTY()
	AAIController* AiController;
	//Walk Speed
	UPROPERTY(EditAnywhere, Category= "CPP Settings|AI Navigation")
	float PatrollingWalkSpeed = 120.f;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|AI Navigation")
	float MaxChasingWalkSpeed = 350.f;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|AI Navigation")
	float MinChasingWalkSpeed = 140.f;
	//Patrol
	int CurrentPatrolTargetIndex = 0;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|AI Navigation")
	EPatrolMethod PatrolMethod; //Different Navigation Methods
	UPROPERTY(VisibleInstanceOnly, Category = "CPP Settings|AI Navigation")
	AActor* PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "CPP Settings|AI Navigation")
	TArray<AActor*> PatrolTargets;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|AI Navigation")
	float PatrolTargetRadius = 200.f;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|AI Navigation")
	float PatrolIdleTime = 4.f;
	//Combat
	UPROPERTY(EditAnywhere, Category= "CPP Settings|Combat Properties")
	float ChaseTargetRadius = 2100.f;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|Combat Properties")
	float AttackTargetRadius = 120.f;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|Combat Properties")
	float AttackMinTimeGap = 0.5f;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|Combat Properties")
	float AttackMaxTimeGap = 1.f;
	/* _Navigation Begin */

	/* Timer */
	void PatrolTimeFinished(); //When the enemy finishes patrol and ready to go to next location, call this function
	void StartAttackTimer();
	void ClearPatrolTimer();
	void ClearAttackTimer();
	void StartHealthBarDelayTimer();
	
	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;
	FTimerHandle HealthBarDelayTimer;
	/* _Timer */
};
