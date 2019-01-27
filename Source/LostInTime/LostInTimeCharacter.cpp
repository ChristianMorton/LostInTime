// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LostInTimeCharacter.h"
#include "LostInTimeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Components/SpotLightComponent.h"
#include "Public/Pickups/Torch.h"
#include "Public/Pickups/RifleWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);



//////////////////////////////////////////////////////////////////////////
// ALostInTimeCharacter

ALostInTimeCharacter::ALostInTimeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	AttachPoint = "GripPoint";

}

void ALostInTimeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ADefaultPickup* NewWeapon = GetWorld()->SpawnActor<ADefaultPickup>(DefaultInventoryClasses[i], SpawnInfo);
			Inventory.AddUnique(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		Inventory[1]->SetOwningPawn(this);
		Inventory[1]->AttachMeshToPawn();
		CurrentWeapon = Inventory[1];
		ECurrentWeapon = EWeaponType::Rifle;
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void ALostInTimeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind jump events
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALostInTimeCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ALostInTimeCharacter::Fire);

	// Bind jump events
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ALostInTimeCharacter::Reload);

	//We tell the compiler that we pick the EWeaponType::Knife explicitly
	InputComponent->BindAction("Torch_Equip", IE_Pressed, this, &ALostInTimeCharacter::SwapWeapon<EWeaponType::Torch>);
	//We tell the compiler that we pick the EWeaponType::Rifle explicitly
	InputComponent->BindAction("Rifle_Equip", IE_Pressed, this, &ALostInTimeCharacter::SwapWeapon<EWeaponType::Rifle>);
	//We tell the compiler that we pick the EWeaponType::Handgun explicitly
	InputComponent->BindAction("Shotgun_Equip", IE_Pressed, this, &ALostInTimeCharacter::SwapWeapon<EWeaponType::Shotgun>);


	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ALostInTimeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALostInTimeCharacter::MoveRight);



	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALostInTimeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALostInTimeCharacter::LookUpAtRate);
}







class USkeletalMeshComponent* ALostInTimeCharacter::GetFPMesh()
{
	return Mesh1P;
}

void ALostInTimeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALostInTimeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALostInTimeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALostInTimeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALostInTimeCharacter::SwapWeapon(EWeaponType EWeaponToEquip)
{
	// equip first weapon in inventory
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		Inventory[i]->DetachMeshFromPawn();
	}
	switch (EWeaponToEquip)
	{
	case EWeaponType::Torch:

		if (Inventory.Num() >= 1)
		{
			Inventory[0]->SetOwningPawn(this);
			Inventory[0]->AttachMeshToPawn();
			CurrentWeapon = Inventory[0];
			ECurrentWeapon = EWeaponType::Torch;
		}
		break;
	case EWeaponType::Rifle:
		if (Inventory.Num() >= 2)
		{
			Inventory[1]->SetOwningPawn(this);
			Inventory[1]->AttachMeshToPawn();
			CurrentWeapon = Inventory[1];
			ECurrentWeapon = EWeaponType::Rifle;
		}
		break;
	case EWeaponType::Shotgun:
		if (Inventory.Num() >= 3)
		{
			Inventory[2]->SetOwningPawn(this);
			Inventory[2]->AttachMeshToPawn();
			CurrentWeapon = Inventory[2];
			ECurrentWeapon = EWeaponType::Shotgun;
		}
		break;
	}
}

void ALostInTimeCharacter::Fire()
{
	if (CurrentWeapon && (ECurrentWeapon == EWeaponType::Rifle || ECurrentWeapon == EWeaponType::Shotgun))
	{
		CurrentWeapon->Fire();

	}
}

void ALostInTimeCharacter::Reload()
{
	if (CurrentWeapon && (ECurrentWeapon == EWeaponType::Rifle || ECurrentWeapon == EWeaponType::Shotgun))
	{
		CurrentWeapon->Reload();
	}
}