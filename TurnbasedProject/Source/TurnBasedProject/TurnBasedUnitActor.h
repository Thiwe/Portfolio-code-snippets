// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnBasedUnitActor.generated.h"

class UCapsuleComponent;

UCLASS()
class TURNBASEDPROJECT_API ATurnBasedUnitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnBasedUnitActor();

	// mesh component to viualise actor in world
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// capsule component to detect mouse over events
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Player")
	int32 UnitActorIndex;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// gettter for root component
	FORCEINLINE UStaticMeshComponent* GetRootComponent() const { return MeshComponent; }
	FORCEINLINE int32 GetUnitActorIndex() const { return UnitActorIndex; }

protected:




};
