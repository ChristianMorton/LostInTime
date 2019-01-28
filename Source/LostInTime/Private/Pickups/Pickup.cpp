// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LostInTimeCharacter.h"
#include "LostInTime.h"



// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapCapsule = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapCapsule"));
	OverlapCapsule->SetGenerateOverlapEvents(true);

	OverlapCapsule->SetCollisionResponseToAllChannels(ECR_Block);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	


}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	OverlapCapsule->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlapBegin);

}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<AActor*> ActorSet;
	OverlapCapsule->GetOverlappingActors(ActorSet, ALostInTimeCharacter::StaticClass());
	for (int32 i = 0; i < ActorSet.Num(); i++)
	{
		ALostInTimeCharacter * temp = Cast<ALostInTimeCharacter>(ActorSet[i]);
		temp->SetPickup(PickupValue);
		Destroy();
	}
}

