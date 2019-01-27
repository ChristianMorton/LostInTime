// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/DefaultPickup.h"
#include "../../LostInTimeCharacter.h"

// Sets default values
ADefaultPickup::ADefaultPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADefaultPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefaultPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefaultPickup::SetOwningPawn(ALostInTimeCharacter* OwningCharacter)
{
	if (MyPawn != OwningCharacter)
	{
		Instigator = OwningCharacter;
		MyPawn = OwningCharacter;
	}
}

void ADefaultPickup::AttachMeshToPawn()
{
	
}

void ADefaultPickup::DetachMeshFromPawn()
{

}

void ADefaultPickup::Fire()
{

}

void ADefaultPickup::Reload()
{
}

