// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurnBasedPlayerController.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "TurnBasedGameMode.generated.h"

UCLASS()
class TURNBASEDPROJECT_API ATurnBasedGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	//override the function to choose a player start location
	// Override ChoosePlayerStart
	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	// Function to choose a player start location based on player index
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void InitializeSpawnPoints();
	
	//function to start the game
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void CycleTurns();

protected:
	// Server function implementation
	void serverRunFunction_Implementation();

private:
	// Map to store player index and spawn point
	TMap<int32, APlayerStart*> SpawnPoints;

	int currentPlayersIndex = 0;
	ATurnBasedPlayerController* PlayerInTurn();
	ATurnBasedPlayerController* PlayerNotInTurn();






};
