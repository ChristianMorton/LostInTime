// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Public/PickupAndRotateActor/PickupAndRotateActor.h"
#include "LostInTimeCharacter.generated.h"

class ATorch;
class UInputComponent;

enum class EWeaponType : uint8
{
	Torch,
	Rifle,
	Shotgun,
	None
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

	class APickup* WeaponPickup;


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	void Fire();

	void Reload();


public:
	ALostInTimeCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	FName GetPickupAttachPoint() { return AttachPoint; }

	class USkeletalMeshComponent* GetFPMesh();

	/** Holding Component */
	UPROPERTY(EditAnywhere)
		class USceneComponent* HoldingComponent;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetPickup(int32 PickupValue);


protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
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

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	bool bHasTorch;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	bool bHasRifle;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	bool bHasShotgun;

	void setHasTorch(bool hasTorch) { bHasTorch = hasTorch; }
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

		/** Action Function */
	void OnAction();

	/** Inspect Function */
	void OnInspect();
	void OnInspectReleased();

	void ToggleMovement();

	void ToggleItemPickup();


public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
	UPROPERTY(EditAnywhere)
		class APickupAndRotateActor* CurrentItem;

	bool bCanMove;
	bool bHoldingItem;
	bool bInspecting;
	float PitchMax;
	float PitchMin;

	FVector HoldingComp;
	FRotator LastRotation;

	FVector Start;
	FVector ForwardVector;
	FVector End;

	FHitResult Hit;

	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParam;

};

