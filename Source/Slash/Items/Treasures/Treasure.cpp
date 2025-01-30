// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasures/Treasure.h"

#include "Interfaces/PickUpInterface.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure() {
	PrimaryActorTick.bCanEverTick = true;
}

void ATreasure::BeginPlay() {
	Super::BeginPlay();
}

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IPickUpInterface* PickUpActor = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpActor->AddGolds(this);
		SpawnPickUpSound();
		Destroy();
	}
}

void ATreasure::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
