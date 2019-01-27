// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "ShotgunWeapon.generated.h"



/**
 * 
 */
UCLASS()
class LOSTINTIME_API AShotgunWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
protected:

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FInstantWeaponData InstantConfig;

	/** instant hit notify*/
	UPROPERTY()
		FInstantHitInfo HitNotify;

	/** current spread from continuous firing */
	float CurrentFiringSpread;

	void Fire();

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	bool ShouldDealDamage(AActor * TestActor) const;
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);
	void NewNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);
	float GetCurrentSpread() const;
};
