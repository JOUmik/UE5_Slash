// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroomComponent.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/PickUpInterface.h"
#include "SlashCharacter.generated.h"

class ATreasure;
class USlashOverlayWidget;
class UTargetComponent;
enum class EActionState : uint8;
enum class ECharacterState : uint8;
class AItem;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;


UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickUpInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	void InitializeSlashOverlayWidget(const APlayerController* PlayerController);
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	FVector GetCameraLocation() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting")
	UInputMappingContext* MappingContext;

	UPROPERTY(BlueprintReadOnly)
	float MovementInputX = 0;
	UPROPERTY(BlueprintReadOnly)
	float MovementInputY = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CPP Settings|Actor Attributes")
	float DodgeStaminaCost = 20.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CPP Settings|Actor Attributes")
	float AttackStaminaCost = 15.f;
	
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE virtual void SetOverlappingItem(AItem* Item) override { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	
	/* Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringComp;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	USlashOverlayWidget* SlashOverlayWidget;
	
	/* Components */
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere)
	UGroomComponent* HairGroom;
	UPROPERTY(VisibleAnywhere)
	UGroomComponent* EyebrowsGroom;
	UPROPERTY(VisibleAnywhere)
	UTargetComponent* TargetComp;
	/* _Components */

	/* Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* HeavyAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
	UInputAction* FocusAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CPP Settings|Input Setting", meta = (AllowPrivateAccess = "true"))
    UInputAction* DodgeAction;
	/* _Input Action */
	
	/* Callbacks for Input */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void UseHeavyAttack(const FInputActionValue& Value);
	void UseNormalAttack(const FInputActionValue& Value);
	void Focus();
	void Dodge(const FInputActionValue& Value);
	/* _Callbacks for Input */

	/*	Behaviours */
	bool bHeavyAttack = false;
	void EquipWeapon(AWeapon* Weapon);
	virtual void Die_Implementation() override;
	virtual void AddSouls(ASoul* Soul) override; /*<IPickUpInterface>*/
	virtual void AddGolds(ATreasure* Treasure) override; /*<IPickUpInterface>*/
	
	/* Animation Montages */
	UPROPERTY(EditDefaultsOnly, Category = "CPP Settings|Montages")
	UAnimMontage* EquipMontage;
	/* _Animation Montages */
	
	/* Play Montage Functions */
	void PlayEquipMontage(FName SectionName);
	virtual void PlayDodgeMontage(const float Angle) override;
	/* _Play Montage Functions */
	
	/* BlueprintCallable Functions Used for AnimInstance */
	virtual void AttackEnd() override;
	UFUNCTION(BlueprintCallable)
	void EquipEnd();
	UFUNCTION(BlueprintCallable)
	void DodgeEnd();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void HitReactFinished();
	/* _BlueprintCallable Functions Used for AnimInstance */
	
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	bool CanDodge();
	void Disarm();
	bool StaminaEnough(float StaminaCost);
	UFUNCTION(BlueprintCallable)
	void SetStaminaPercent(float StaminaCost);
	void Arm();
	
	UPROPERTY(VisibleInstanceOnly, Category = "CPP Settings|Instance Only")
	AItem* OverlappingItem;

	/* UENUM */
	ECharacterState CharacterState;
	EActionState ActionState;
	/* _UENUM */

	/* Timer */
	FTimerHandle HealthBarDelayTimer;
	FTimerHandle StaminaBarDelayTimer;
	FTimerHandle StaminaRecoveryTimer;
	FTimerHandle DodgingTimer;
	/* _Timer */
};
