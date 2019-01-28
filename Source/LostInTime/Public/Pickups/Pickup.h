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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Capsule)
	class USphereComponent* OverlapCapsule;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = Pickup)
		int8 PickupValue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
