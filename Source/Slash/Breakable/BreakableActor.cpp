// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableActor.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasures/Treasure.h"
#include "Kismet/GameplayStatics.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollectionComp = CreateDefaultSubobject<UGeometryCollectionComponent>(FName("Geometry Collection Comp"));
	ChaosGameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(FName("Chaos Gameplay Event Dispatcher"));
	
	SetRootComponent(GeometryCollectionComp);
	
	GeometryCollectionComp->SetGenerateOverlapEvents(true);
	GeometryCollectionComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollectionComp->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		BreakSound,
		ImpactPoint
	);

	if(TreasureClasses.Num()>0)
	{
		const int32 index = FMath::RandRange(0, TreasureClasses.Num() - 1);
		FVector Location = GetActorLocation();
		Location.Z += 50;
		GetWorld()->SpawnActor<ATreasure>(TreasureClasses[index], Location, GetActorRotation());
	}
	
}