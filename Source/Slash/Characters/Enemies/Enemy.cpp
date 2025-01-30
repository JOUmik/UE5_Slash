// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Characters/Player/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"

#include "Characters/CharacterTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "AIController.h"
#include "HUD/TargetPointComponent.h"
#include "Items/Soul/Soul.h"

AEnemy::AEnemy() : CurrentState(EEnemyState::EES_Patrolling), PatrolMethod(EPatrolMethod::EPM_BackAndForth)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	/*
	 * Components
	 */
	HealthBarComp = CreateDefaultSubobject<UHealthBarComponent>("Health Bar Component");
	HealthBarComp->SetupAttachment(GetRootComponent());
	TargetPointComp = CreateDefaultSubobject<UTargetPointComponent>("Target Point Component");
	TargetPointComp->SetupAttachment(GetRootComponent());
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");

	PawnSensingComp->SightRadius = 1600.f;
	PawnSensingComp->SetPeripheralVisionAngle(45.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 120.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(MaxChasingWalkSpeed < MinChasingWalkSpeed)
	{
		MaxChasingWalkSpeed = MinChasingWalkSpeed;
	}
}

void AEnemy::HitReactEnd()
{
	ClearAttackTimer();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AiController = Cast<AAIController>(GetController());
	Tags.AddUnique(ENEMY_TAG);

	if(AiController && PatrolTargets.Num()>0)
	{
		PatrolTarget = PatrolTargets[0];
		MoveToTarget(PatrolTarget);
	}

	if(PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);
	}

	//Attach the weapon to enemy
	if(WeaponClass)
	{
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		EquippedWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If already died, do nothing
	if(IsDead())
	{
		return;
	}

	//Patrol State
	if(IsPatrolling())
	{
		UpdatePatrol();
	}
	//Combat State(chasing, attacking,engaged and ...)
	else
	{
		UpdateCombat();
	}
}

void AEnemy::UpdatePatrol()
{
	//if this enemy has at least two different patrol locations and already arrived at one location, update patrol target to move to a new location
	if(InTargetRange(PatrolTarget, PatrolTargetRadius))
	{
		if(PatrolMethod == EPatrolMethod::EPM_Circle)
		{
			Patrol_Circle();
		}
		else if(PatrolMethod == EPatrolMethod::EPM_BackAndForth)
		{
			Patrol_BackAndForth();
		}
		else if(PatrolMethod == EPatrolMethod::EPM_OneWay)
		{
			Patrol_OneWay();
		}
		else if(PatrolMethod == EPatrolMethod::EPM_Random)
		{
			Patrol_Random();
		}
	}
}

void AEnemy::UpdateCombat()
{
	//If player out of range, lose interest and back to patrol
	if(IsOutsideCombatRadius() || CombatTarget->ActorHasTag(Dead_TAG))
	{
		LostInterest();
		ClearAttackTimer();
		//Back to patrol state, wait for a while and move back to the last patrol location
		if(!IsEngaged()) StartPatrolling();
	}
	//outside attack radius, chase player
	else if(IsOutsideAttackRadius())
	{
		ClearAttackTimer();
		if(!IsEngaged()) ChaseTarget();
	}
	else
	{
		if(!IsAttacking())
		{
			AiController->StopMovement();
			if(CanAttack())
			{
				ClearAttackTimer();
				StartAttackTimer();
			}
		}
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if(IsAlive())
	{
		ChangeCurrentState(EEnemyState::EES_Chasing);
	}
	ClearAttackTimer();
	
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	//Play HitMontage
	if(IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	//Play DeathMontage
	else
	{
		Die();	
	}
}

void AEnemy::Select()
{
	TargetPointComp->ShowTargetPoint();
}

void AEnemy::CancelSelect()
{
	TargetPointComp->HideTargetPoint();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	HandleDamage(DamageAmount);

	if(!GetWorldTimerManager().TimerExists(HealthBarDelayTimer))
	{
		GetWorldTimerManager().SetTimer(HealthBarDelayTimer, this, &AEnemy::StartHealthBarDelayTimer, 1.f);
	}

	if(DamageCauser->GetOwner()->ActorHasTag(ENEMY_TAG)) return DamageAmount;  // if the enemy is hit by another enemy, do not anger
	
	CombatTarget = EventInstigator->GetPawn();
	
	if(CurrentState == EEnemyState::EES_Patrolling)
	{
		ChangeCurrentState(EEnemyState::EES_Chasing);
		
		ClearPatrolTimer();
	}
	

	return DamageAmount;
}

void AEnemy::HandleDamage(const float DamageAmount) const
{
	Super::HandleDamage(DamageAmount);

	if(HealthBarComp && AttributeComp->IsAlive())
	{
		HealthBarComp->SetHealthPercent(AttributeComp->GetHealthPercent());
	}
	else if(HealthBarComp)
	{
		HealthBarComp->OwnerDead();
	}
}

void AEnemy::OnSeePawn(APawn* Pawn)
{
	if(Pawn->ActorHasTag(Dead_TAG)) return;
	if(Pawn->ActorHasTag(SLASH_CHARACTER_TAG) && CurrentState == EEnemyState::EES_Patrolling)
	{
		CombatTarget = Pawn;
		ChangeCurrentState(EEnemyState::EES_Chasing);
		//clear timer
		ClearPatrolTimer();
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if(!AiController || !Target) return;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(6.f);
	AiController->MoveTo(MoveRequest);
}

void AEnemy::Die_Implementation()
{
	if(CurrentState == EEnemyState::EES_Dead) return;
	//if dead, no collision with other pawn, including player
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	Tags.AddUnique(Dead_TAG);
	
	//Stop move
	if(AiController)
	{
		AiController->StopMovement();
	}
	
	ClearPatrolTimer();
	ClearAttackTimer();
	
	ChangeCurrentState(EEnemyState::EES_Dead);

	PlayDeathMontage();

	//Spawn Soul
	if(UWorld* World = GetWorld())
	{
		if(SoulClass)
		{
			const FVector SpawnLocation = GetActorLocation() + SpawnSoulOffset;
			ASoul* Soul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
			if(Soul)
			{
				Soul->SetSouls(SoulValue);
			}
			
		}
	}
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 SelectedIndex = Super::PlayDeathMontage();
	
	switch (SelectedIndex)
	{
	case 0:
		DeathPose = EDeathPose::EDP_Death1;
		break;
	case 1:
		DeathPose = EDeathPose::EDP_Death2;
		break;
	case 2:
		DeathPose = EDeathPose::EDP_Death3;
		break;
	case 3:
		DeathPose = EDeathPose::EDP_Death4;
		break;
	case 4:
		DeathPose = EDeathPose::EDP_Death5;
		break;
	case 5:
		DeathPose = EDeathPose::EDP_Death6;
		break;
	default:
		DeathPose = EDeathPose::EDP_Death1;
		break;
	}
	return SelectedIndex;
}

bool AEnemy::InTargetRange(AActor* TargetActor, const double Radius)
{
	if(!TargetActor) return false;
	const float DistanceToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).Size();
	if(DistanceToTarget <= Radius)
	{
		return true;
	}
	
	return false;
	
}

float AEnemy::UpdateCombatWalkSpeed() const
{
	if(!CombatTarget) return MaxChasingWalkSpeed;
	const float Distance = (CombatTarget->GetActorLocation() - GetActorLocation()).Length();
	if(Distance <= 500.f)
	{
		return MinChasingWalkSpeed;
	}
	if(Distance >= 2000.f)
	{
		return MaxChasingWalkSpeed;
	}
	const float LogBase = FMath::Log2(1.f);
	const float LogTop = FMath::Log2(2000.f - 499.f);
	const float LogCur = FMath::Log2(Distance - 499.f);
	//(350.f - 140.f) / (LogTop - LogBase) * (LogCur - LogBase) + 140.f
	const float Speed = (MaxChasingWalkSpeed - MinChasingWalkSpeed) / (LogTop - LogBase) * (LogCur - LogBase) + MinChasingWalkSpeed;
	return Speed;
}

void AEnemy::Patrol_Circle()
{
	//This patrol method would not idle
	if(AiController && PatrolTargets.Num()>1)
	{
		CurrentPatrolTargetIndex++;
		CurrentPatrolTargetIndex %= PatrolTargets.Num();
		
		PatrolTarget = PatrolTargets[CurrentPatrolTargetIndex];
		
		MoveToTarget(PatrolTarget);
	}
}

void AEnemy::Patrol_BackAndForth()
{
	//This patrol method would idle in the both ends of the route
	if(AiController && PatrolTargets.Num()>1)
	{
				
		//If the enemy arrives at the end, idle for a while
		if(CurrentPatrolTargetIndex == 0 || CurrentPatrolTargetIndex == PatrolTargets.Num()-1)
		{
			bIsIdle = true;
			AiController->StopMovement();
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimeFinished, PatrolIdleTime);
		}
		//Update PatrolTarget
		//0 --> PatrolTargets.Num()-1
		if(!bIsReverse_BackAndForth)
		{
			CurrentPatrolTargetIndex++;
			if(CurrentPatrolTargetIndex == PatrolTargets.Num()-1)
			{
				bIsReverse_BackAndForth = true;
			}
		}
		//PatrolTargets.Num()-1 --> 0
		else
		{
			CurrentPatrolTargetIndex--;
			if(CurrentPatrolTargetIndex == 0)
			{
				bIsReverse_BackAndForth = false;
			}
		}
		PatrolTarget = PatrolTargets[CurrentPatrolTargetIndex];
		//if the enemy not arrives at the end, continue walk
		if(!bIsIdle)
		{
			MoveToTarget(PatrolTarget);
		}
	}
}

void AEnemy::Patrol_OneWay()
{
	//This patrol method would reach the end and keeps on idling
	if(AiController && PatrolTargets.Num()>1 && CurrentPatrolTargetIndex < PatrolTargets.Num()-1)
	{
		CurrentPatrolTargetIndex++;
		PatrolTarget = PatrolTargets[CurrentPatrolTargetIndex];
		MoveToTarget(PatrolTarget);
	}
}

void AEnemy::Patrol_Random()
{
	//This patrol method would randomly move and every time reach a new location,  idle for a while
	if(AiController && PatrolTargets.Num()>1)
	{
		AiController->StopMovement();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimeFinished, PatrolIdleTime);
		//Update PatrolTarget
		TArray<AActor*> OtherTargets;
		for(auto Target:PatrolTargets)
		{
			if(Target != PatrolTarget)
			{
				OtherTargets.AddUnique(Target);
			}
		}
		CurrentPatrolTargetIndex = FMath::RandRange(0, OtherTargets.Num()-1);
		
		PatrolTarget = PatrolTargets[CurrentPatrolTargetIndex];
		
	}
}

void AEnemy::PatrolTimeFinished()
{
	//If this enemy has at least two different target locations, then move to another one, or stay idle
	if(PatrolTargets.Num()>1)
	{
		//Move to new location
		MoveToTarget(PatrolTarget);
		bIsIdle = false;
	}
}

void AEnemy::StartAttackTimer()
{
	ChangeCurrentState(EEnemyState::EES_Attacking);
	const float AttackTime = FMath::RandRange(AttackMinTimeGap, AttackMaxTimeGap);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::StartHealthBarDelayTimer()
{
	if(HealthBarComp)
	{
		HealthBarComp->SetHealthBarDelay();
	}
}

void AEnemy::ChangeCurrentState(EEnemyState NewState)
{
	if(CurrentState == NewState) return;
	CurrentState = NewState;
	if(CurrentState == EEnemyState::EES_Patrolling)
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrollingWalkSpeed;
	}
	else if(CurrentState == EEnemyState::EES_Chasing)
	{
		GetCharacterMovement()->MaxWalkSpeed = UpdateCombatWalkSpeed();;
	}
	else if(CurrentState == EEnemyState::EES_Attacking)
	{
		GetCharacterMovement()->MaxWalkSpeed = MinChasingWalkSpeed;
	}
}

void AEnemy::LostInterest()
{
	CombatTarget = nullptr;
}

void AEnemy::StartPatrolling()
{
	ChangeCurrentState(EEnemyState::EES_Patrolling);
	AiController->StopMovement();
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimeFinished, 3.f);
}

void AEnemy::ChaseTarget()
{
	ChangeCurrentState(EEnemyState::EES_Chasing);
	GetCharacterMovement()->MaxWalkSpeed = UpdateCombatWalkSpeed();
	MoveToTarget(CombatTarget);
}

void AEnemy::Attack()
{
	Super::Attack();
	ChangeCurrentState(EEnemyState::EES_Engaged);
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return CurrentState != EEnemyState::EES_Engaged;
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();

	//Engaged state to chasing state
	ChangeCurrentState(EEnemyState::EES_Chasing);
}

bool AEnemy::IsPatrolling() const
{
	return CurrentState == EEnemyState::EES_Patrolling;
}

bool AEnemy::IsChasing() const
{
	return CurrentState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking() const
{
	return CurrentState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged() const
{
	return CurrentState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsDead() const
{
	return CurrentState == EEnemyState::EES_Dead;
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, ChaseTargetRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackTargetRadius);
}