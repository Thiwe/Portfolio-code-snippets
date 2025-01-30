#include "TurnBasedGameMode.h"
#include "TurnBasedPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include "EngineUtils.h" // Required for TActorIterator
#include "TurnBasedPlayerStart.h"

class ATurnBasedPlayerController;

ATurnBasedPlayerController* ATurnBasedGameMode::PlayerInTurn()
{
    //get the player controller of the given index
    ATurnBasedPlayerController* PlayerController = Cast<ATurnBasedPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), currentPlayersIndex));
    
    return PlayerController;
}

ATurnBasedPlayerController* ATurnBasedGameMode::PlayerNotInTurn()
{
    //if the current players index is 0 then return 1 if the current players index is 1 then return 0 then get the player controller of the given index
    ATurnBasedPlayerController* PlayerController = Cast<ATurnBasedPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), currentPlayersIndex == 0 ? 1 : 0));
    return PlayerController;
}

AActor* ATurnBasedGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    InitializeSpawnPoints(); // Ensure spawn points are initialized

    // Get the number of players
    int32 NumberOfPlayers = UGameplayStatics::GetGameMode(GetWorld())->GetNumPlayers();

    // Find the spawn point for the given number of players
    APlayerStart** SpawnPointFound = SpawnPoints.Find(NumberOfPlayers);
    if (SpawnPointFound)
    {
        return *SpawnPointFound;
    }

    // Fallback to the default behavior if no spawn point is found
    return Super::ChoosePlayerStart_Implementation(Player);
}

void ATurnBasedGameMode::OnPostLogin(AController* NewPlayer)
{
    //get number of platers
    int32 NumberOfPlayers = UGameplayStatics::GetGameMode(GetWorld())->GetNumPlayers();

    ATurnBasedPlayerController* PlayerController = Cast<ATurnBasedPlayerController>(NewPlayer);
    if (PlayerController)
	{
		// Set the player index
        PlayerController->SetPlayerControllerIndex(NumberOfPlayers - 1);
	}

    //if the number of players is greater than 1
    if (NumberOfPlayers > 1)
	{
        StartGame();
	}

}

void ATurnBasedGameMode::InitializeSpawnPoints()
{
    if (SpawnPoints.Num() == 0)
    {
        // Iterate over all instances of ATurnBasedPlayerStart in the world
        for (TActorIterator<ATurnBasedPlayerStart> It(GetWorld()); It; ++It)
        {
            ATurnBasedPlayerStart* FoundPlayerStart = *It;
            if (FoundPlayerStart)
            {
                int32 PlayerIndex = FoundPlayerStart->GetPlayerIndex();
                SpawnPoints.Add(PlayerIndex, FoundPlayerStart);
            }
        }
    }
}

void ATurnBasedGameMode::StartGame()
{
    // call the startTurn function on the player in turn
    PlayerInTurn()->Client_StartTurn();
    
}

void ATurnBasedGameMode::CycleTurns()
{
    // call the endTurn function on the player in turn
	PlayerInTurn()->Client_EndTurn();

    currentPlayersIndex = currentPlayersIndex == 0 ? 1 : 0;
	// call the startTurn function on the player not in turn
	PlayerInTurn()->Client_StartTurn();
}


