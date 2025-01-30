// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"

#include "PhysicsEngine/RadialForceComponent.h"
#include "SProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class URadialForceComponent;
class FTimerManager;
class UParticleSystem;
class UStaticMeshComponent;


UCLASS()
class GAMEPLAYMECHANICS_API ASProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	ASProjectile();

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	URadialForceComponent* GetExplosionForce() const { return ExplosionForce; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* MeshComponent;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	FTimerHandle ExplosionTimerHandle;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(	UPrimitiveComponent* HitComp, 
				AActor* OtherActor, 
				UPrimitiveComponent* OtherComp, 
				FVector NormalImpulse, 
				const FHitResult& Hit );
	UFUNCTION()
	void Explode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	URadialForceComponent* ExplosionForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float ActorLifeSpan;

};
