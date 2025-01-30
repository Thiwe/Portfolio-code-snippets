// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSController.h"
#include "SuperhotProjectile.h"
#include "SuperhotFPSController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYMECHANICS_API ASuperhotFPSController : public AFPSController
{
	GENERATED_BODY()
public:
	ASuperhotFPSController();

	USceneComponent* ProjectileSpawnPos; 
protected:
	virtual void BeginPlay() override;
	float HAxis;
	float VAxis;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Shoot();

	virtual void MoveForward(float input) override;
	virtual void MoveRight(float input) override;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<ASuperhotProjectile> ProjectileClass; 
};
