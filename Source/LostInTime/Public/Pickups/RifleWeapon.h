// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Public/Pickups/BaseWeapon.h"
#include "RifleWeapon.generated.h"



USTRUCT()
struct FMagazineData
{
	GENERATED_USTRUCT_BODY()

public:
		/** Ammo in the current mag */
		int32 CurrentAmmo;

	FMagazineData()
	{
		CurrentAmmo = 10;
	}
};

/**
 * 
 */
UCLASS()
class LOSTINTIME_API ARifleWeapon : public ABaseWeapon
{
	GENERATED_BODY()

		ARifleWeapon();

		TArray<FMagazineData*> Magazines;

		void AddMagazine();

		int32 CurrentMagazine = 0;



protected:

	virtual void UseAmmo() override;
	
	/** current spread from continuous firing */
	float CurrentFiringSpread;

	/** get current spread */
	float GetCurrentSpread() const;

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FInstantWeaponData InstantConfig;

	/** instant hit notify*/
	UPROPERTY()
		FInstantHitInfo HitNotify;

	/** check if weapon should deal damage to actor */
	bool ShouldDealDamage(AActor* TestActor) const;

	/** handle damage */
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	UFUNCTION()
	void NewNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);


public:
	virtual void Reload() override;


	virtual void Fire() override;

	int32 GetCurrentMagazine() { return CurrentMagazine; }

	bool CanFire();

};
