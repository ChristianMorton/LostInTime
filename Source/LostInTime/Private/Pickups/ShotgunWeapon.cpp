// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/ShotgunWeapon.h"
#include "LostInTimeCharacter.h"
#include "LostInTime.h"



void AShotgunWeapon::Fire()
{
	for (int32 i = 0; i < InstantConfig.ShotgunNumberOfPellets; i++) {
		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float CurrentSpread = GetCurrentSpread();
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

		const FVector AimDir = GetAdjustedAim();
		const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
		const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
		const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

		const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
		ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
	}

	//CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);

}


void AShotgunWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor())
		{
			// notify the server of the hit
			NewNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				NewNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			}

		}
	}

// 	// process a confirmed hit
// 	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}


bool AShotgunWeapon::ShouldDealDamage(AActor* TestActor) const
{
	return true;
}

void AShotgunWeapon::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}
}

void AShotgunWeapon::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = InstantConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}


void AShotgunWeapon::NewNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	if (Impact.GetActor()) {
		const FVector Origin = GetMuzzleLocation();
		ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
	}
	
}

float AShotgunWeapon::GetCurrentSpread() const
{
	float FinalSpread = InstantConfig.WeaponSpread + CurrentFiringSpread;
	return FinalSpread;
}