// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



//#include "SProjectile.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "C:/Program Files/Epic Games/UE_4.25/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "C:/Program Files/Epic Games/UE_4.25/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "TurretTargetComponent.h"

#include "TurretController.generated.h"

UCLASS()
class GAMEPLAYMECHANICS_API ATurretController : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	FTimerHandle FireDelay;
	/*The ProgressFunction contains the signature of the function that is
	going to execute every time we tick our timeline.
	Think of the progressfunction as a delegate!*/
	
public:	
	// Sets default values for this actor's properties
	ATurretController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float TurretLineOfSightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float MaxVerticalRotation = -40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float MaxHorizontalRotation =-60;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float RotationSpeed = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float RateOfFire = 0.2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    AActor* CurrentTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float IdleRotationSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float Spread = 60;
	UPROPERTY()
	float DeltaTimeVar = 0;
	UPROPERTY()
	FTimeline MyTimeline;
	UPROPERTY()
	FOnTimelineFloat ProgressFunction;
	UPROPERTY(EditAnywhere, Category="Timeline")
	UCurveFloat* FCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NiagaraBeamEnd", meta = (MakeEditWidget = true))
	FVector ProjectileBeamEnd = FVector::ZeroVector;

	/*The function that will handle every tick of the float curve*/
	UPROPERTY()
	float TimelineAlphaOut = 0;
	UFUNCTION()
    void HandleTimelineUpdate(float Value);
	UFUNCTION()
	void OnLostTarget();
	UFUNCTION()
	void OnHasTarget();
	
	/*
	 *Turret will get all actors that go within its line of sight and ask if it has the enemy tag, if it has it will shoot at it.
	 * will have to have a firendly turret later and enemy turret.  
	 * 
	 */
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "SceneComponent")
	USceneComponent* Scene;
	UPROPERTY(EditAnywhere, Category = "SceneComponent")
	USceneComponent* TurretRotation;
	UPROPERTY(EditAnywhere, Category = "SceneComponent")
	USceneComponent* MuzzleFlashLocation;
	//shot and impact FX
	UPROPERTY(EditAnywhere, Category = "Projectilesystem")
	UParticleSystemComponent* FX_MuzzleFlash;
	UPROPERTY(EditAnywhere, Category = "Projectilesystem")
	UNiagaraSystem* SystemTemplate;
	UPROPERTY(EditAnywhere, Category = "Projectilesystem")
	UParticleSystemComponent* ProjectileClass;
	UPROPERTY(EditAnywhere, Category = "Projectilesystem")
	UParticleSystem* ImpactEffect;
		
	UPROPERTY(EditAnywhere, Category = "SceneComponent")
	USceneComponent* LineOfSightPivot;
	UPROPERTY(EditAnywhere, Category = "TurretLineOfSight")
	USphereComponent* TurretLineOfSight;
	UPROPERTY()
	UArrowComponent* ArrowComp;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	//returns target if supplied actor has tag enemy.
	UTurretTargetComponent* GetTurretTargetComponent(AActor* CurrentTargetActor);
	UFUNCTION()
	void Fire();
	UFUNCTION()
	AActor* FindFirstTarget();
	UFUNCTION()
	void OnLineOfSightBeginOverlap(	class UPrimitiveComponent* OverlappedComp,
									class AActor* OtherActor,
									class UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex,
									bool bFromSweep,
									const FHitResult& SweepResult);
};
