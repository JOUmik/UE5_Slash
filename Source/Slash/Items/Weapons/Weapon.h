// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

namespace EDrawDebugTrace
{
	enum Type : int;
}

class UBoxComponent;
/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();
	void Equip(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void Disequip(const FVector& PlacedLocation);
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	UPROPERTY(VisibleInstanceOnly, Category = "CPP Settings|Weapon Properties")
	TArray<AActor*> IgnoredActors;
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComp;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStartPoint;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEndPoint;

private:
	void BoxTrace(FHitResult& BoxHit);
	
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Sounds")
	USoundBase* EquipSound;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Weapon Properties")
	float Damage = 20.f;
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Weapon Properties")
	FVector BoxSizeForTrace = FVector(5.f);
	UPROPERTY(EditAnywhere, Category = "CPP Settings|Weapon Properties")
	bool bShowBoxTrace = false;
	
public:
	//Getter and Setter
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return BoxComp; }
};
