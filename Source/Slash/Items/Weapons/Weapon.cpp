// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Characters/Player/SlashCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Breakable/BreakableActor.h"

AWeapon::AWeapon()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("Collision Box Comp"));
	BoxComp->SetupAttachment(SphereComp);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	
	//trace point
	BoxTraceStartPoint = CreateDefaultSubobject<USceneComponent>(FName("Box Trace Start Point"));
	BoxTraceEndPoint = CreateDefaultSubobject<USceneComponent>(FName("Box Trace End Point"));
	BoxTraceStartPoint->SetupAttachment(RootComponent);
	BoxTraceEndPoint->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	ItemState = EIS_Equipped;

	PlayEquipSound();

	if(SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(NiagaraComp)
	{
		NiagaraComp->Deactivate();
	}
}

void AWeapon::Disequip(const FVector& PlacedLocation)
{
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetActorLocation(PlacedLocation);
	ItemState = EIS_Hovering;
	
	if(SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if(NiagaraComp)
	{
		NiagaraComp->Activate(true);
	}
}

void AWeapon::PlayEquipSound()
{
	if(EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
			);
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	StaticMeshComp->AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
}

//Detect whether the weapon hit something
void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult BoxHitResult;
	
	BoxTrace(BoxHitResult);

	if(BoxHitResult.GetActor())
	{
		//ApplyDamage First
		UGameplayStatics::ApplyDamage(
			BoxHitResult.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);
		
		//if the hit actor has used IHitInterface, call GetHit()
		if(IHitInterface* HitInterface = Cast<IHitInterface>(BoxHitResult.GetActor()))
		{
			IgnoredActors.Add(BoxHitResult.GetActor());
			HitInterface->Execute_GetHit(BoxHitResult.GetActor(),BoxHitResult.ImpactPoint, GetOwner());

			if(Cast<ABreakableActor>(BoxHitResult.GetActor()))
			{
				CreateFields(BoxHitResult.ImpactPoint);
			}
			
		}
		
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector StartPoint = BoxTraceStartPoint->GetComponentLocation();
	const FVector EndPoint = BoxTraceEndPoint->GetComponentLocation();

	IgnoredActors.AddUnique(GetOwner()); //do not trace the actor held this weapon
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartPoint,
		EndPoint,
		BoxSizeForTrace,
		BoxTraceStartPoint->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoredActors,
		bShowBoxTrace ? EDrawDebugTrace::Type::ForDuration : EDrawDebugTrace::Type::None,
		BoxHit,
		true
		);
}
