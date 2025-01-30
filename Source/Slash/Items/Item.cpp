// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickUpInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(FName("Niagara Component"));
	
	RootComponent = StaticMeshComp;
	SphereComp->SetupAttachment(RootComponent);
	NiagaraComp->SetupAttachment(RootComponent);

	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime+=DeltaTime;

	if(ItemState == EIS_Hovering)
	{
		SinMovement();
	}
}

void AItem::SinMovement()
{
	AddActorWorldOffset(FVector(0,0, Amplitude * FMath::Sin(RunningTime * TimeConstant)));
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IPickUpInterface* PickUpActor = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpActor->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(IPickUpInterface* PickUpActor = Cast<IPickUpInterface>(OtherActor))
	{
		PickUpActor->SetOverlappingItem(nullptr);
	}
}

void AItem::SpawnPickUpSystem()
{
	if(PickUpEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickUpEffect,
			GetActorLocation()
		);
	}
}

void AItem::SpawnPickUpSound()
{
	if(PickUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickUpSound,
			GetActorLocation()
        );
	}
}
