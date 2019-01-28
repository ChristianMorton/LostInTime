// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LostInTimeCharacter.generated.h"

class ATorch;
class UInputComponent;

enum class EWeaponType : uint8
{
	Torch,
	Rifle,
	Shotgun
};

UCLASS(config=Game)
class ALostInTimeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = Pickups)
	FName AttachPoint;

	/** default inventory list */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ADefaultPickup> > DefaultInventoryClasses;

	/** Items in inventory */
	UPROPERTY()
		TArray<class ADefaultPickup*> Inventory;

	class ADefaultPickup* CurrentWeapon;


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	void Fire();

	void Reload();


public:
	ALostInTimeCharacter();

protected:
	virtual void BeginPlay();

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	FName GetPickupAttachPoint() { return AttachPoint; }

	class USkeletalMeshComponent* GetFPMesh();

	void SetPickup(int8 PickupValue);


protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	EWeaponType ECurrentWeapon;

	void SwapWeapon(EWeaponType EWeaponToEquip);

	template<EWeaponType EWeaponToEquip>
	void SwapWeapon() { SwapWeapon(EWeaponToEquip); }

	bool bHasTorch;
	bool bHasRifle;
	bool bHasShotgun;

	void setHasTorch(bool hasTorch) { bHasTorch = hasTorch; }
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

