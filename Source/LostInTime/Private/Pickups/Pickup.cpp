// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LostInTimeCharacter.h"
#include "LostInTime.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	


}

void APickup::GiveWeaponToCharacter()
{
	ALostInTimeCharacter* tmp = Cast<ALostInTimeCharacter>(MyCharacter);
	tmp->SetPickup(PickupValue);
	if (tmp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast complete!"));
	}
	Destroy();
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	


}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

