// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"

#include "fighterController.generated.h"

UCLASS()
class GAMEPLAYMECHANICS_API AfighterController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AfighterController();

	bool localBool = false;
	float DefaultCameraBoomarmLength = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* shipMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FighterCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnComponents")
	UStaticMeshSocket* SpawnPoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float input);
	virtual void MoveRight(float input);
	virtual void MoveUp(float input);
	virtual void MovePitch(float input);
	virtual void MoveRoll(float input);
	virtual void MoveYaw(float input);
	virtual void ZoomCamera(float input);

	virtual void BoostPressed();
	virtual void BoostReleased();

	virtual void ChangeCamera();

	bool freeLook = false;
	FRotator DefaultCameraBoomRotation = FRotator(0.0, 0.0, 0.0);
	virtual void LookAroundPressed();
	virtual void LookAroundReleased();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Engine")
	float TorquePower = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Engine")
	float ThrusterPower = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Engine")
	float BoostMultiplier = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Engine")
	float AngularDampening = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Engine")
	float LinearDampening = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Engine")
	float NrOfThrusters;
	UFUNCTION(BlueprintCallable, Category = "Engine")
	void UpdateSideThrusters(UStaticMeshComponent* ThrusterMesh);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthPercentage;


};
