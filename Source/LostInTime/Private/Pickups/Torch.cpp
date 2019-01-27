// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/Torch.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../../LostInTimeCharacter.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ATorch::ATorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TorchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchMesh"));
	RootComponent = TorchMesh;
	TorchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
	Light->Activate(true);
	Light->AttachToComponent(TorchMesh, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void ATorch::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATorch::AttachMeshToPawn()
{
	if (MyPawn) {
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetPickupAttachPoint();
		USkeletalMeshComponent* PawnMesh = MyPawn->GetFPMesh();

		TorchMesh->AttachToComponent(PawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	}

}

void ATorch::DetachMeshFromPawn()
{
	TorchMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

// Called every frame
void ATorch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

