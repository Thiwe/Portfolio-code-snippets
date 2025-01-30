// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"



// Sets default values
ASProjectile::ASProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnHit);	// set up a notification for when this component hits something blocking
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	//CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Set as root component
	RootComponent = CollisionComp;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(CollisionComp);
	MeshComponent->CastShadow = true;
	MeshComponent->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
	//MeshComponent->RelativeRotation = FRotator(2.0f, -15.0f, 5.0f);
	//MeshComponent->RelativeLocation = FVector(0, 0, -160.0f);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	ExplosionForce->AddCollisionChannelToAffect(ECC_WorldDynamic);
	ExplosionForce->AddCollisionChannelToAffect(ECC_PhysicsBody);
	ExplosionForce->AddCollisionChannelToAffect(ECC_WorldStatic);
	ExplosionForce->Radius = 200.0f;
	ExplosionForce->ForceStrength = 2000000.0f;
	ExplosionForce->bIgnoreOwningActor = true;
	ExplosionForce->bAutoActivate = false;

	// Die after 4 seconds by default
	ActorLifeSpan = 4.f;

}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ASProjectile::Explode, 10, false, 3.0f);
}

void ASProjectile::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{

}

void ASProjectile::Explode()
{
	ExplosionForce->FireImpulse();

	//UGameplayStatics::ApplyRadialDamage()
	if (DefaultImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, this->GetActorLocation(), this->GetActorRotation(), true);  //GetWorld(), DefaultImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
	Destroy();
}