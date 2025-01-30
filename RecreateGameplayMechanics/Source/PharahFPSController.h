// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LetsCreateFPS/Public/FPSController.h"
#include "PharahFPSController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYMECHANICS_API APharahFPSController : public AFPSController
{
	GENERATED_BODY()

public:
    APharahFPSController();

protected:
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Shoot();

	// virtual void MoveForward(float input) override;
	// virtual void MoveRight(float input) override;

	void JetPackPressed();
	void JetPackReleased();
	void ShiftAbility();
	void ResetBoost();
	void JetPackAbility();

	bool bIsJetPacking;
	bool bCanBoost;
	int32 FuelAmount; 
	float MaxFuelAmount;

	FTimerHandle BoostDelayTimerHandle;
	FTimerHandle JetPackAbilityTimerHandle;

	
};
