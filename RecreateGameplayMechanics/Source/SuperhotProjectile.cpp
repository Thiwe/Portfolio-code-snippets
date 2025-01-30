// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsCreateFPS/Public/SuperhotProjectile.h"

// Sets default values
ASuperhotProjectile::ASuperhotProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Component"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetWorldScale3D(FVector(0.48, 0.2, 0.2));
	MeshComp->SetSimulatePhysics(true);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionBox->InitBoxExtent(FVector(60.0, 60.0, 60.0));
	CollisionBox->SetupAttachment(MeshComp);
	CollisionBox->SetSimulatePhysics(true);
	
}

// Called when the game starts or when spawned
void ASuperhotProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASuperhotProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

