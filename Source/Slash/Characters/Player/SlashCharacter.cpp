// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Characters/CharacterTypes.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/TargetComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlayWidget.h"
#include "Interfaces/ICharacterState.h"
#include "Items/Soul/Soul.h"
#include "Items/Treasures/Treasure.h"
#include "Kismet/KismetMathLibrary.h"
#include "State/IdleState.h"
#include "State/AttackState.h"
#include "State/DodgeState.h"
#include "State/JumpState.h"
#include "State/MoveState.h"
#include "State/DodgeState.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//set default UseControllerRotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//Camera
	SpringComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));

	SpringComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringComp);

	//Groom
	HairGroom = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair Groom"));
	EyebrowsGroom = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows Groom"));

	HairGroom->SetupAttachment(GetMesh());
	EyebrowsGroom->SetupAttachment(GetMesh());

	HairGroom->AttachmentName = FString("head");
	EyebrowsGroom->AttachmentName = FString("head");

	//Target
	TargetComp = CreateDefaultSubobject<UTargetComponent>(TEXT("Target Component"));

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_Unoccupied;

	Tags.AddUnique(SLASH_CHARACTER_TAG);
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(MappingContext, 0);
		}

		InitializeSlashOverlayWidget(PlayerController);
	}

	//state
	idleState = new IdleState();
	moveState = new MoveState();
	jumpState = new JumpState();
	attackState = new AttackState();
	dodgeState = new DodgeState();
	CurrentState = idleState;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(SlashOverlayWidget && SlashOverlayWidget->StaminaRecover && AttributeComp)
	{
		AttributeComp->SetStaminaByPercent(SlashOverlayWidget->GetStaminaBarPercent());
	}

	CurrentState->UpdateState(this, DeltaTime);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputComp) {
		InputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		InputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		InputComp->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::StartJump);
		InputComp->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASlashCharacter::StopJump);
		InputComp->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressed);
		InputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::AttackKey);
		InputComp->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ASlashCharacter::UseHeavyAttack);
		InputComp->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &ASlashCharacter::UseNormalAttack);
		InputComp->BindAction(FocusAction, ETriggerEvent::Started, this, &ASlashCharacter::Focus);
		InputComp->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::Dodge);
	}
}

void ASlashCharacter::InitializeSlashOverlayWidget(const APlayerController* PlayerController)
{
	//Get SlashOverlayWidget
	if(const ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD()))
	{
		SlashOverlayWidget = SlashHUD->GetSlashOverlayWidget();
		if(SlashOverlayWidget)
		{
			SlashOverlayWidget->SetGolds(0);
			SlashOverlayWidget->SetSouls(0);
			SlashOverlayWidget->SetHealthBarPercent(1.f);
			SlashOverlayWidget->SetStaminaBarPercent(1.f);
		}
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HandleDamage(DamageAmount);

	if(SlashOverlayWidget)
	{
		SlashOverlayWidget->SetHealthBarPercent(AttributeComp->GetHealthPercent());
		if(!GetWorldTimerManager().TimerExists(HealthBarDelayTimer))
		{
			GetWorldTimerManager().SetTimer(HealthBarDelayTimer, SlashOverlayWidget, &USlashOverlayWidget::SetHealthBarDelay, 1.f);
		}
		
	}

	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	//do not disturb dodging montage
	if(ActionState == EActionState::EAS_Dodging) return;
	
	//Play HitMontage
	if(IsAlive() && Hitter)
	{
		ActionState = EActionState::EAS_HitReacting;
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	//Play DeathMontage
	else
	{
		Die();	
	}
}

FVector ASlashCharacter::GetCameraLocation() const
{
	return CameraComp->GetComponentLocation();
}

float ASlashCharacter::GetGroundSpeed() const
{
	return UKismetMathLibrary::VSizeXY(GetMovementComponent()->Velocity);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	const FVector2d Input = Value.Get<FVector2d>();
	MovementInputX = Input.X;
	MovementInputY = Input.Y;
	
	if(CharacterState == ECharacterState::ECS_Dead) return;
	if(ActionState != EActionState::EAS_Unoccupied) return;
	StopAnimMontage();
	
	if(Controller)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Input.Y);
		AddMovementInput(RightDirection, Input.X);
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	FVector2d Input = Value.Get<FVector2d>();

	if (Controller)
	{
		// add yaw and pitch input to controller
		if(!TargetComp->bIsFocusing) AddControllerYawInput(Input.X);
		AddControllerPitchInput(Input.Y);
	}
}

void ASlashCharacter::StartJump(const FInputActionValue& Value)
{
	if(CharacterState == ECharacterState::ECS_Dead) return;
	if(ActionState != EActionState::EAS_Unoccupied) return;
	StopAnimMontage();
	if(GetCharacterMovement()->IsMovingOnGround())
	{
		Jump();
	}
}

void ASlashCharacter::StopJump(const FInputActionValue& Value)
{
	if(CharacterState == ECharacterState::ECS_Dead) return;
	if(ActionState != EActionState::EAS_Unoccupied) return;
	StopJumping();
}

void ASlashCharacter::EKeyPressed()
{
	if(CharacterState == ECharacterState::ECS_Dead) return;
	//If the character doesn't have a weapon, take the weapon
	if(AWeapon* Weapon = Cast<AWeapon>(OverlappingItem))
	{
		EquipWeapon(Weapon);
	}
	//if the character arms the weapon and is not attacking, then put the weapon to the back
	else if(CanDisarm())
	{
		Disarm();
	}
	//if the character disarms the weapon, then arms the weapon
	else if(CanArm())
	{
		Arm();
	}
}

void ASlashCharacter::Attack()
{
	if(CharacterState == ECharacterState::ECS_Dead) return;
	
	Super::Attack();
	if(CanAttack())
	{
		if(bHeavyAttack)
		{
			PlayMontageSection(AttackMontage, FName("Attack2"));
		}
		else
		{
			PlayMontageSection(AttackMontage, FName("Attack1"));
		}
		ActionState = EActionState::EAS_Attacking;
		ChangeState(attackState);
	}
}

void ASlashCharacter::UseHeavyAttack(const FInputActionValue& Value)
{
	bHeavyAttack = true;
}

void ASlashCharacter::UseNormalAttack(const FInputActionValue& Value)
{
	bHeavyAttack = false;
}

void ASlashCharacter::Focus()
{
	if(TargetComp)
	{
		TargetComp->FindOrCancelTarget();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Have No TargetComponent")));
	}
	
}
void ASlashCharacter::Dodge()
{
	if(CanDodge())
	{
		if(TargetComp->bIsFocusing)
		{
			//If is focusing, free rotation mode temporarily
			TargetComp->FreeModeRotation();
			TargetComp->DodgeCameraLagOpen();
		}
		PlayDodgeMontage(0.f);
		//GetWorldTimerManager().SetTimer(DodgingTimer, this, &ASlashCharacter::PlayDodgeMontage, GetWorld()->GetDeltaSeconds());
		ChangeState(dodgeState);
	}
}
void ASlashCharacter::AttackKey(const FInputActionValue& Value)
{
	attackState->HandleInput(this, Value);
}

void ASlashCharacter::DodgeKey(const FInputActionValue& Value)
{
	dodgeState->HandleInput(this, Value);
}


void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	//Already have weapon, replace
	if(EquippedWeapon)
	{
		const FVector NewLocation = Weapon->GetActorLocation();
		EquippedWeapon->Disequip(NewLocation);
		EquippedWeapon = Weapon;
		if(CharacterState == ECharacterState::ECS_Disarm)
		{
			Weapon->Equip(GetMesh(), FName("SpineSocket"), this, this);
		}
		else
		{
			Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		}
	}
	else
	{
		EquippedWeapon = Weapon;
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	//TargetComponent->CancelFocusTarget();
	Tags.AddUnique(Dead_TAG);
	CharacterState = ECharacterState::ECS_Dead;

	//if player dead, clear target and return
	TargetComp->CancelFocusTarget();
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if(AttributeComp)
	{
		AttributeComp->AddSouls(Soul->GetSouls());
	}
	if(SlashOverlayWidget)
	{
		SlashOverlayWidget->AddSouls(Soul->GetSouls());
	}
}

void ASlashCharacter::AddGolds(ATreasure* Treasure)
{
	if(AttributeComp)
	{
		AttributeComp->AddGolds(Treasure->GetGold());
	}
	if(SlashOverlayWidget)
	{
		SlashOverlayWidget->AddGolds(Treasure->GetGold());
	}
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if(EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage);

			AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
		}
	}
}

void ASlashCharacter::PlayDodgeMontage(const float Angle)
{
	double Yaw = 0.f;
	if(TargetComp->bIsFocusing)
	{
		//rotate player to the correct direction
		FRotator CurRotation = GetMesh()->GetRelativeRotation();
		Yaw = FMath::Atan2(1.f, 0.f) - FMath::Atan2(MovementInputY, MovementInputX);
		Yaw = FMath::RadiansToDegrees(Yaw);
		CurRotation.Yaw += Yaw;
		GetMesh()->SetRelativeRotation(CurRotation);
	}
	
	Super::PlayDodgeMontage(Yaw);

	ActionState = EActionState::EAS_Dodging;
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	ChangeState(idleState);
}

void ASlashCharacter::EquipEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	ChangeState(idleState);

	if(TargetComp->bIsFocusing)
	{
		TargetComp->FocusingModeRotation();
		TargetComp->DodgeCameraLagClose();
	}
}

void ASlashCharacter::AttachWeaponToBack()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::HitReactFinished()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped &&
		CharacterState != ECharacterState::ECS_Disarm &&
		StaminaEnough(AttackStaminaCost);
}

bool ASlashCharacter::CanDisarm()
{
	return CharacterState != ECharacterState::ECS_Unequipped &&
		CharacterState != ECharacterState::ECS_Disarm &&
		ActionState == EActionState::EAS_Unoccupied &&
		GetCharacterMovement()->IsMovingOnGround();
}

bool ASlashCharacter::CanArm()
{
	return CharacterState == ECharacterState::ECS_Disarm &&
		ActionState == EActionState::EAS_Unoccupied &&
		ActionState != EActionState::EAS_HitReacting &&
		ActionState != EActionState::EAS_Dodging &&
		GetCharacterMovement()->IsMovingOnGround();
}

bool ASlashCharacter::CanDodge()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		StaminaEnough(DodgeStaminaCost);
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Disarm;
	ActionState = EActionState::EAS_Equipping;
}

bool ASlashCharacter::StaminaEnough(float StaminaCost)
{
	if(!AttributeComp) return false;
	if(!AttributeComp->CanUseStamina(StaminaCost)) return false;
	return true;
}

void ASlashCharacter::SetStaminaPercent(float StaminaCost)
{
	if(!AttributeComp) return;
	AttributeComp->CostStamina(StaminaCost);
	const float Percent = AttributeComp->GetStaminaPercent();
	if (SlashOverlayWidget)
	{
		SlashOverlayWidget->StopRecovery();
		SlashOverlayWidget->SetStaminaBarPercent(Percent);
		if(!GetWorldTimerManager().TimerExists(StaminaBarDelayTimer))
		{
			GetWorldTimerManager().SetTimer(StaminaBarDelayTimer, SlashOverlayWidget, &USlashOverlayWidget::SetStaminaBarDelay, 1.f);
		}
		GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
		GetWorldTimerManager().SetTimer(StaminaRecoveryTimer, SlashOverlayWidget, &USlashOverlayWidget::StartRecovery, 2.f);
	}
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_Equipping;
}

void ASlashCharacter::ChangeState(ICharacterState* NewState)
{
	if (CurrentState)
	{
		CurrentState->ExitState(this);
	}
	CurrentState = NewState;
	if (CurrentState)
	{
		CurrentState->EnterState(this);
	}
}

ICharacterState* ASlashCharacter::GetIdleState() const
{
	return idleState;
}

ICharacterState* ASlashCharacter::GetJumpState() const
{
	return jumpState;
}

ICharacterState* ASlashCharacter::GetMoveState() const
{
	return moveState;
}

ICharacterState* ASlashCharacter::GetAttackState() const
{
	return attackState;
}

ICharacterState* ASlashCharacter::GetDodgeState() const
{
	return dodgeState;
}
