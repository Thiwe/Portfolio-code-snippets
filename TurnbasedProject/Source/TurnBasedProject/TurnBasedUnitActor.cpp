// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedUnitActor.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ATurnBasedUnitActor::ATurnBasedUnitActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(MeshComponent);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->SetCapsuleSize(50.0f, 100.0f);
	CapsuleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	CapsuleComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		

}

// Called when the game starts or when spawned
void ATurnBasedUnitActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurnBasedUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



