// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DefaultPickup.generated.h"

UCLASS()
class LOSTINTIME_API ADefaultPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADefaultPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		class ALostInTimeCharacter* MyPawn;

	

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//Sets the owner of the object4
	void SetOwningPawn(class ALostInTimeCharacter* OwningCharacter);
	virtual void AttachMeshToPawn();
	virtual void DetachMeshFromPawn();

	virtual void Fire();

	virtual void Reload();

};
