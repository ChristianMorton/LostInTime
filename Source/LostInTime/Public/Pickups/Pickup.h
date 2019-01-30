// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

//DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnOverlapDelegate, AActor*, MyOverlappedActor, AActor*, OtherActor);


UCLASS()
class LOSTINTIME_API APickup : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	APickup();



	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = Pickup)
		int8 PickupValue;



	void GiveWeaponToCharacter();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class ACharacter* MyCharacter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
