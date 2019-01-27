// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultPickup.h"
#include "Torch.generated.h"

UCLASS()
class LOSTINTIME_API ATorch : public ADefaultPickup
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorch();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Torch)
		class UStaticMeshComponent* TorchMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torch)
	class USpotLightComponent* Light;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void AttachMeshToPawn() override;
	virtual void DetachMeshFromPawn() override;

};
