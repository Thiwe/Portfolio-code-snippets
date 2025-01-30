// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSController.h"
#include "TracerFPSController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYMECHANICS_API ATracerFPSController : public AFPSController
{
	GENERATED_BODY()

public:
	ATracerFPSController();
	void SetTracerPosition();
	void ResetAbility();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bCanSetPos;
	bool bCanRecall;
	bool bRecallPressed;

	int32 Counter;
	float RecallSpeed;
	/**
	 * the time between setting recall points
	 */
	float TimeBeforeSettingNewPos;

	FTimerHandle SetPosTimerHandle;
	FTimerHandle RecallCooldownTimerHandle;
	
	TArray<FTransform> RecallPositions;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Ability();
	
	
};
