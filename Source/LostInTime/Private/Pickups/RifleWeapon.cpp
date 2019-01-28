// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pickups/RifleWeapon.h"
#include "LostInTime.h"
#include "../../LostInTimeCharacter.h"
#include "../../LostInTime.h"

ARifleWeapon::ARifleWeapon()
{
	for (int32 i = 0; i < WeaponConfig.InitialClips; i++)
	{
		FMagazineData* NewMagazine = new FMagazineData;
		NewMagazine->CurrentAmmo = WeaponConfig.AmmoPerClip;
		Magazines.Add(NewMagazine);
	}

	CurrentFiringSpread = 0.0f;
}

void ARifleWeapon::AddMagazine()
{
	FMagazineData* NewMagazine = new FMagazineData;
	NewMagazine->CurrentAmmo = WeaponConfig.AmmoPerClip;
	Magazines.Add(NewMagazine);

}

void ARifleWeapon::Fire()
{
	if (CanFire()) {
		UseAmmo();
		UE_LOG(LogWeapon, Warning, TEXT("Current mag is %d"), GetCurrentMagazine());
		UE_LOG(LogWeapon, Warning, TEXT("Current ammo in mag is %d"), Magazines[GetCurrentMagazine()]->CurrentAmmo);

		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float CurrentSpread = GetCurrentSpread();
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

		const FVector AimDir = GetAdjustedAim();
		const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
		const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
		const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

		const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
		if(Impact.GetActor())
			ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

		//CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);
	}

}

bool ARifleWeapon::CanFire()
{
	
	if (Magazines[GetCurrentMagazine()]->CurrentAmmo > 0)
	{
		return true;
	}
	else
		return false;
}

void ARifleWeapon::UseAmmo()
{
// 	if (Magazines[GetCurrentMagazine()]->CurrentAmmo == 0)
// 	{
// 		Reload();
// 		
// 	}
// 	if (Magazines[GetCurrentMagazine()]->CurrentAmmo == 1 && Magazines.Num() == 1)
// 	{
// 		UE_LOG(LogWeapon, Warning, TEXT("Out of mags!"));
// 
// 	}
	if (Magazines[GetCurrentMagazine()]->CurrentAmmo > 0)
	{
		Magazines[GetCurrentMagazine()]->CurrentAmmo--;
	}

	
	
}

float ARifleWeapon::GetCurrentSpread() const
{
	float FinalSpread = InstantConfig.WeaponSpread + CurrentFiringSpread;
	return FinalSpread;
}

void ARifleWeapon::Reload()
{
	if (Magazines.Num() != 1) {
		if (Magazines[GetCurrentMagazine()]->CurrentAmmo <= 0)
			Magazines.RemoveSingle(Magazines[GetCurrentMagazine()]);
	}
	int32 randomMagToSwitchTo = FMath::RandRange(0, Magazines.Num() - 1);
	CurrentMagazine = randomMagToSwitchTo;

	UE_LOG(LogWeapon, Warning, TEXT("Current mag is %d"), GetCurrentMagazine());
}


bool ARifleWeapon::ShouldDealDamage(AActor* TestActor) const
{
	if (TestActor->GetClass()->IsChildOf(APawn::StaticClass()))
	{
		return true;
	}
	else
		return false;
}

void ARifleWeapon::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = InstantConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = InstantConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
	
}

void ARifleWeapon::NewNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{

	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		{
			
				if (Impact.GetActor() == NULL)
				{
					if (Impact.bBlockingHit)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				}
				// assume it told the truth about static things because the don't move and the hit 
				// usually doesn't have significant gameplay implications
				else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				}
				else
				{
					// Get the component bounding box
					const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

					// calculate the box extent, and increase by a leeway
					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);

					// avoid precision errors with really thin objects
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

					// Get the box center
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

					// if we are within client tolerance
					if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
						FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
						FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
					else
					{
						UE_LOG(LogWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
					}
				}
			
		}
		else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		{
			UE_LOG(LogWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
		else
		{
			UE_LOG(LogWeapon, Log, TEXT("%s Rejected client side hit of %s"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
	}
}

void ARifleWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
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

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

void ARifleWeapon::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

}

