// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class ATreasure;
class UChaosGameplayEventDispatcher;
class UGeometryCollectionComponent;
UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollectionComp;
	UPROPERTY(VisibleAnywhere)
	UChaosGameplayEventDispatcher* ChaosGameplayEventDispatcher;

private:
	UPROPERTY(EditAnywhere, Category="CPP Settings|Sounds")
	USoundBase* BreakSound;

	UPROPERTY(EditDefaultsOnly, Category = "CPP Settings|Class")
	TArray<TSubclassOf<ATreasure>> TreasureClasses;
};
