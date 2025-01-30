// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	
	UPROPERTY(VisibleAnywhere, Category = "CPP Settings|Weapon")
	AWeapon* EquippedWeapon;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	/*
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComp;
	/* _Components */
	
	void PlayHitSound(const FVector& ImpactPoint) const;
	void SpawnHitParticle(const FVector& ImpactPoint) const;
	
	/* Behaviours */
	bool IsAlive() const;
	virtual void Attack();
	virtual bool CanAttack();
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	virtual void HandleDamage(const float DamageAmount) const;
	/* _Behaviours */
	
	UPROPERTY(BlueprintReadOnly, Category = "CPP Settings|Combat Properties")
	AActor* CombatTarget;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Combat Properties")
	double WarpTargetDistance = 40.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "CPP Settings|Montages")
	UAnimMontage* AttackMontage;
	/*
	 * Play Montage Functions
	 */
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) const;
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames) const;
	virtual int32 PlayAttackMontage() const;
	virtual int32 PlayDeathMontage();
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void PlayDodgeMontage(const float Angle);
	/* _Play Montage Functions */

	/* Motion Warping */
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
	/* _Motion Warping */
	
private:
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Sounds")
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category= "CPP Settings|VFX")
	UParticleSystem* HitParticle;
	
	/* Animation Montages */
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Montages")
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Montages")
	TArray<FName> DeathMontageSections;
	
	UPROPERTY(EditDefaultsOnly, Category = "CPP Settings|Montages")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "CPP Settings|Montages")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "CPP Settings|Montages")
	UAnimMontage* DodgeMontage;
	/* _Animation Montages */
};
