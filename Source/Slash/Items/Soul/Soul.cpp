// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul/Soul.h"

#include "Components/SphereComponent.h"
#include "Interfaces/PickUpInterface.h"

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->SetGenerateOverlapEvents(false);
	GetWorldTimerManager().SetTimer(EnableOverlappingEvent, this, &ASoul::EnableOverlap, GetWorld()->GetDeltaSeconds());
}

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IPickUpInterface* PickUpActor = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpActor->AddSouls(this);

		SpawnPickUpSystem();
		SpawnPickUpSound();

		Destroy();
	}
}

void ASoul::EnableOverlap()
{
	SphereComp->SetGenerateOverlapEvents(true);
}
