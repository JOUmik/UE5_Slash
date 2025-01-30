// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USphereComponent;

enum EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SinMovement();

	EItemState ItemState = EIS_Hovering;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComp;
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "CPP Settings|VFX")
	UNiagaraSystem* PickUpEffect;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Sounds")
	USoundBase* PickUpSound;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void SpawnPickUpSystem();
	virtual void SpawnPickUpSound();
	
private:
	
	float RunningTime = 0.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Sin Movement", Meta = (AllowPrivateAccess = "true"))
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Sin Movement", Meta = (AllowPrivateAccess = "true"))
	float TimeConstant = 5.f;
};
