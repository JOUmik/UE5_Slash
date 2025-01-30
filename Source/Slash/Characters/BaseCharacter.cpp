// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComp = CreateDefaultSubobject<UAttributeComponent>("Attribute Component");
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponCollisionBox())
	{
		EquippedWeapon->GetWeaponCollisionBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoredActors.Empty();
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint) const
{
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnHitParticle(const FVector& ImpactPoint) const
{
	if(HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			ImpactPoint,
			FRotator::ZeroRotator,
			FVector(0.7)
		);
	}
}

void ABaseCharacter::Attack()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	/*
	 * Get the Angle to figure out which montage section should be used
	 *
	 * Only want to get the angle of XY axis, ignore Z axis
	 */
	const FVector Forward = GetActorForwardVector();

	//Ignore Z axis and normalize the vector
	FVector ToHit = ImpactPoint - GetActorLocation();
	ToHit.Z = 0;
	ToHit = ToHit.GetSafeNormal();
	
	/*
	 * Dot Product:
	 *
	 *	Forward · ToHit = |Forward|*|ToHit|*Cos(theta)
	 *	|Forward| == 1 and |ToHit| == 1, so
	 *	Forward · ToHit = Cos(theta)
	 *	
	 */
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);

	//convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if(CrossProduct.Z<=0)
	{
		Theta = -Theta;	
	}

	if(Theta <= 45.f && Theta > -45.f)
	{
		PlayHitReactMontage(FName("FromFront"));
	}
	else if(Theta <= 135.f && Theta > 45.f)
	{
		PlayHitReactMontage(FName("FromRight"));
	}
	else if(Theta <= -135.f || Theta > 135.f)
	{
		PlayHitReactMontage(FName("FromBack"));
	}
	else
	{
		PlayHitReactMontage(FName("FromLeft"));
	}

	/*
	 * Draw Debug
	 */
	//UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+Forward*80,5.f,FColor::Orange,5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+ToHit*80,5.f,FColor::Purple,5.f);
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::Die_Implementation()
{
}

bool ABaseCharacter::IsAlive() const
{
	return AttributeComp && AttributeComp->IsAlive();
}

void ABaseCharacter::HandleDamage(const float DamageAmount) const
{
	if(AttributeComp)
	{
		AttributeComp->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) const
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if(Montage)
		{
			AnimInstance->Montage_Play(Montage);
			AnimInstance->Montage_JumpToSection(SectionName, Montage);
		}
		
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames) const
{
	if(Montage && SectionNames.Num() > 0)
	{
		const int32 MaxSectionIndex = SectionNames.Num() - 1;
		const int32 SelectedIndex = FMath::RandRange(0, MaxSectionIndex);

		PlayMontageSection(Montage, SectionNames[SelectedIndex]);

		return SelectedIndex;
	}
	return -1;
}

int32 ABaseCharacter::PlayAttackMontage() const
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if(HitReactMontage)
		{
			AnimInstance->Montage_Play(HitReactMontage);

			AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
		}
	}
}

void ABaseCharacter::PlayDodgeMontage(const float Angle)
{
	if(Angle >= -45.f && Angle <= 45.f)
	{
		PlayMontageSection(DodgeMontage, FName("DodgeFront"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Dodge Front: %f"), Angle));
	}
	else if(Angle < -45.f && Angle >= -135.f)
	{
		PlayMontageSection(DodgeMontage, FName("DodgeRight"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Dodge Right: %f"), Angle));
	}
	else if(Angle < 135.f && Angle > 45.f)
	{
		PlayMontageSection(DodgeMontage, FName("DodgeLeft"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Dodge Left: %f"), Angle));
	}
	else
	{
		PlayMontageSection(DodgeMontage, FName("DodgeBack"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Dodge Back: %f"), Angle));
	}
	
}

//move base on the location vector in front of the combat target location to make sure can hit the combat target
FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(CombatTarget)
	{
		const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
		const FVector Location = GetActorLocation();
		
		FVector TargetToSelf = (Location - CombatTargetLocation).GetSafeNormal();
		TargetToSelf *= WarpTargetDistance;
		
		return CombatTargetLocation + TargetToSelf;
	}
	return FVector::Zero();
}

//rotate base on the location vector passed
FVector ABaseCharacter::GetRotationWarpTarget()
{
	if(CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector::Zero();
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	//if is playing attack montage, disable weapon collision first
	if(GetCurrentMontage() == AttackMontage)
	{
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	PlayHitSound(ImpactPoint);

	SpawnHitParticle(ImpactPoint);
}
