// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultPickup.h"
#include "BaseWeapon.generated.h"

USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		FVector Origin;

	UPROPERTY()
		float ReticleSpread;

	UPROPERTY()
		int32 RandomSeed;
};

USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

		/** base weapon spread (degrees) */
		UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float WeaponSpread;

	/** targeting spread modifier */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float TargetingSpreadMod;

	/** continuous firing: spread increment */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadIncrement;

	/** continuous firing: max increment */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadMax;

	/** weapon range */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float WeaponRange;

	/** damage amount */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		int32 HitDamage;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		int32 ShotgunNumberOfPellets;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		TSubclassOf<UDamageType> DamageType;

	/** hit verification: threshold for dot product between view direction and hit direction */
	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
		float AllowedViewDotHitDir;

	/** defaults */
	FInstantWeaponData()
	{
		WeaponSpread = 5.0f;
		TargetingSpreadMod = 0.25f;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		WeaponRange = 10000.0f;
		HitDamage = 10;
		DamageType = UDamageType::StaticClass();
		AllowedViewDotHitDir = 0.8f;
		ShotgunNumberOfPellets = 5;
	}
};


USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

		/** inifite ammo for reloads */
		UPROPERTY(EditDefaultsOnly, Category = Ammo)
		bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		bool bInfiniteClip;

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 InitialClips;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float TimeBetweenShots;

	/** Number of shots to be fired at once */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		int32 NumberOfShots;

	/** defaults */
	FWeaponData()
	{
		bInfiniteAmmo = false;
		bInfiniteClip = false;
		MaxAmmo = 100;
		AmmoPerClip = 20;
		InitialClips = 4;
		TimeBetweenShots = 0.2f;
		NumberOfShots = 1;
	}
};


/**
 * 
 */
UCLASS()
class LOSTINTIME_API ABaseWeapon : public ADefaultPickup
{
	GENERATED_BODY()


public:
	ABaseWeapon();

protected:

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	class USkeletalMeshComponent* Mesh1P;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FWeaponData WeaponConfig;

	virtual void AttachMeshToPawn() override;

	virtual void DetachMeshFromPawn() override;

	

	virtual void UseAmmo();

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		FName MuzzleAttachPoint;

	bool CanReload();

	int32 GetMaxAmmo();

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;
	FVector GetAdjustedAim() const;
	FVector GetMuzzleLocation() const;
public:
	virtual void Fire() override;

	virtual void Reload();

	bool CanFire();

	
};
