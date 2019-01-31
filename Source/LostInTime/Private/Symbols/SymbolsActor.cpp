// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Symbols/SymbolsActor.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))

// Sets default values
ASymbolsActor::ASymbolsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = Decal;


	//Register Events
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASymbolsActor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASymbolsActor::OnOverlapEnd);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerBox->SetupAttachment(Decal);

}

// Called when the game starts or when spawned
void ASymbolsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASymbolsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASymbolsActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// check if Actors do not equal nullptr and that 
	if (OtherActor && (OtherActor != this)) {
		// print to screen using above defined method when actor enters trigger box
		print("Overlap Begin");
		printFString("Overlapped Actor = %s", *OtherActor->GetName());
	}
}

void ASymbolsActor::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this)) {
		// print to screen using above defined method when actor leaves trigger box
		print("Overlap Ended");
		printFString("%s has left the Trigger Box", *OtherActor->GetName());
	}
}

