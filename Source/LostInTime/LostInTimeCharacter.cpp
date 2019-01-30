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
#include "Public/Pickups/Pickup.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);



//////////////////////////////////////////////////////////////////////////
// ALostInTimeCharacter

ALostInTimeCharacter::ALostInTimeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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

	bHasTorch = false;
	bHasRifle = false;
	bHasShotgun = false;

	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->RelativeLocation.X = 150.0f;
	HoldingComponent->SetupAttachment(FirstPersonCameraComponent);

	CurrentItem = NULL;
	bCanMove = true;
	bInspecting = false;
	bHoldingItem = false;

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
		ECurrentWeapon = EWeaponType::None;
	}

	//Holding item get view pitch
	PitchMax = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax;
	PitchMin = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin;

}

void ALostInTimeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Start = FirstPersonCameraComponent->GetComponentLocation();
	ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	End = ((ForwardVector * 300.f) + Start);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (!bHoldingItem)
	{
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam))
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(APickup::StaticClass()))
			{
				WeaponPickup = Cast<APickup>(Hit.GetActor());
			}
			else if (Hit.GetActor()->GetClass()->IsChildOf(APickupAndRotateActor::StaticClass()))
			{
				
				CurrentItem = Cast<APickupAndRotateActor>(Hit.GetActor());
				
				UE_LOG(LogTemp, Warning, TEXT("Got actor!"));
			}
		}
		else
		{
			CurrentItem = NULL;
			WeaponPickup = NULL;
		}
	}

	if (bInspecting)
	{
		if (bHoldingItem)
		{
			FirstPersonCameraComponent->SetFieldOfView(FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 90.0f, 0.1f));
			HoldingComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 179.9000002f;
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -179.9000002f;
			CurrentItem->RotateActor();
		}
		else
		{
			FirstPersonCameraComponent->SetFieldOfView(FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 45.0f, 0.1f));
		}
	}
	else
	{
		FirstPersonCameraComponent->SetFieldOfView(FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 90.0f, 0.1f));

		if (bHoldingItem)
		{
			HoldingComponent->SetRelativeLocation(FVector(150.0f, 0.0f, 0.f));
		}
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

	// pickup
	// Bind action event
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ALostInTimeCharacter::OnAction);

	// Bind Inspect event
	PlayerInputComponent->BindAction("Inspect", IE_Pressed, this, &ALostInTimeCharacter::OnInspect);
	PlayerInputComponent->BindAction("Inspect", IE_Released, this, &ALostInTimeCharacter::OnInspectReleased);


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

void ALostInTimeCharacter::SetPickup(int32 PickupValue)
{
	switch (PickupValue)
	{
	case 0: 
		bHasTorch = true;
		SwapWeapon(EWeaponType::Torch);
		break;
	case 1:
		bHasRifle = true;
		SwapWeapon(EWeaponType::Rifle);
		break;
	case 2:
		bHasShotgun = true;
		SwapWeapon(EWeaponType::Shotgun);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("PickupValue out of range when calling SetPickup!"));
		break;
	}
}

void ALostInTimeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f && bCanMove)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALostInTimeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f && bCanMove)
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

		if (Inventory.Num() >= 1 && bHasTorch)
		{
			Inventory[0]->SetOwningPawn(this);
			Inventory[0]->AttachMeshToPawn();
			CurrentWeapon = Inventory[0];
			ECurrentWeapon = EWeaponType::Torch;
		}
		break;
	case EWeaponType::Rifle:
		if (Inventory.Num() >= 2 && bHasRifle)
		{
			Inventory[1]->SetOwningPawn(this);
			Inventory[1]->AttachMeshToPawn();
			CurrentWeapon = Inventory[1];
			ECurrentWeapon = EWeaponType::Rifle;
		}
		break;
	case EWeaponType::Shotgun:
		if (Inventory.Num() >= 3 && bHasShotgun)
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

void ALostInTimeCharacter::OnAction()
{
	if (WeaponPickup)
	{
		UE_LOG(LogTemp, Warning, TEXT("Should call GiveWeaponToCharacter"));
		WeaponPickup->GiveWeaponToCharacter();
	}

	if (CurrentItem && !bInspecting)
	{
		ToggleItemPickup();
	}
}

void ALostInTimeCharacter::OnInspect()
{
	if (bHoldingItem)
	{
		LastRotation = GetControlRotation();
		ToggleMovement();
	}
	else
	{
		bInspecting = true;
	}
}

void ALostInTimeCharacter::OnInspectReleased()
{
	if (bInspecting && bHoldingItem)
	{
		GetController()->SetControlRotation(LastRotation);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = PitchMax;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = PitchMin;
		ToggleMovement();
	}
	else
	{
		bInspecting = false;
	}
}

void ALostInTimeCharacter::ToggleMovement()
{
	bCanMove = !bCanMove;
	bInspecting = !bInspecting;
	FirstPersonCameraComponent->bUsePawnControlRotation = !FirstPersonCameraComponent->bUsePawnControlRotation;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
}

void ALostInTimeCharacter::ToggleItemPickup()
{
	if (CurrentItem)
	{
		bHoldingItem = !bHoldingItem;
		CurrentItem->Pickup();

		if (!bHoldingItem)
		{
			CurrentItem = NULL;
		}
	}
}