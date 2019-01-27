// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "LostInTimeCharacter.h"
#include "Public/LostInTimePlayerController.h"
#include "../../LostInTime.h"



ABaseWeapon::ABaseWeapon()
{
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	RootComponent = Mesh1P;
}

void ABaseWeapon::AttachMeshToPawn()
{
	if (MyPawn) {
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetPickupAttachPoint();
		USkeletalMeshComponent* PawnMesh = MyPawn->GetFPMesh();

		Mesh1P->AttachToComponent(PawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	}

}

void ABaseWeapon::DetachMeshFromPawn()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

void ABaseWeapon::Fire()
{
}

void ABaseWeapon::UseAmmo()
{
}

void ABaseWeapon::Reload()
{
}

bool ABaseWeapon::CanFire()
{
	return false;
}

bool ABaseWeapon::CanReload()
{
	return false;
}

int32 ABaseWeapon::GetMaxAmmo()
{
	return WeaponConfig.MaxAmmo;
}


FHitResult ABaseWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{

	const FName TraceTag("MyTraceTag");

	GetWorld()->DebugDrawTraceTag = TraceTag;


	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.TraceTag = TraceTag;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

FVector ABaseWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	ALostInTimePlayerController* PC = MyPawn ? Cast<ALostInTimePlayerController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}

	return OutStartTrace;
}

FVector ABaseWeapon::GetAdjustedAim() const
{
	ALostInTimePlayerController* const PlayerController = Instigator ? Cast<ALostInTimePlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		{
			FinalAim = Instigator->GetBaseAimRotation().Vector();
		}
	}

	return FinalAim;
}

FVector ABaseWeapon::GetMuzzleLocation() const
{
	USkeletalMeshComponent* UseMesh = Mesh1P;
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}