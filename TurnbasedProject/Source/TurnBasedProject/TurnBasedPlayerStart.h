// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "TurnBasedPlayerStart.generated.h"

/**
 *
 */
UCLASS()
class TURNBASEDPROJECT_API ATurnBasedPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	//initialize which player gets to start here
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int32 PlayerIndex;

	//getter function to get the player index from outside
	UFUNCTION(BlueprintPure, Category = "Player")
	int32 GetPlayerIndex() const;





};
