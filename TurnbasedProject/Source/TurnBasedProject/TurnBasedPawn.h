// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurnBasedPawn.generated.h"


class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UArrowComponent;
class UMovementComponent;

UCLASS()
class TURNBASEDPROJECT_API ATurnBasedPawn : public APawn
{
	GENERATED_BODY()

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = sphere, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = sphere, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = sphere, meta = (AllowPrivateAccess = "true"))
	class UMovementComponent* MovementComponent;


public:
	// Sets default values for this pawn's properties
	ATurnBasedPawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//------------------------------------

	//function to get the camera boom
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UMovementComponent* GetTurnBasedMovementComponent() const { return MovementComponent; }
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

private:


};
