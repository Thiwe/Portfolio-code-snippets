// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsCreateFPS/Public/PharahFPSController.h"

#include <string>



#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APharahFPSController::APharahFPSController()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    MaxFuelAmount = 3.0;
    FuelAmount = 0;
    bCanBoost = true;
    
}

void APharahFPSController::BeginPlay()
{
    Super::BeginPlay();
}

void APharahFPSController::Tick(float DeltaTime)
{
    
    Super::Tick(DeltaTime);
    UE_LOG(LogTemp, Warning, TEXT("  %d  "), &FuelAmount);
    
}

void APharahFPSController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APharahFPSController::Shoot);
    
    PlayerInputComponent->BindAction("JetPack", IE_Pressed, this, &APharahFPSController::JetPackPressed);
    PlayerInputComponent->BindAction("JetPack", IE_Released, this, &APharahFPSController::JetPackReleased);
    
    PlayerInputComponent->BindAction("ShiftAbility", IE_Pressed, this, &APharahFPSController::ShiftAbility);
}

void APharahFPSController::Shoot()
{
    UE_LOG(LogTemp, Warning, TEXT("BANG!"))
    this->GetMovementComponent()->Velocity.Z += 5000;
}

void APharahFPSController::JetPackPressed()
{
    bIsJetPacking = true;
    GetWorldTimerManager().SetTimer(JetPackAbilityTimerHandle, this, &APharahFPSController::JetPackAbility, 0.1, true);
}
void APharahFPSController::JetPackReleased()
{
    bIsJetPacking = false;
}

void APharahFPSController::ShiftAbility()
{

    if(bCanBoost)
    {

        bCanBoost = false;
        Jump();
        this->GetMovementComponent()->Velocity.Z = 1500;
        GetWorldTimerManager().SetTimer(BoostDelayTimerHandle, this, &APharahFPSController::ResetBoost, 6, false);
    
    }
}

void APharahFPSController::ResetBoost()
{
    bCanBoost = true;
    UE_LOG(LogTemp, Warning, TEXT(" triggered boost reset "));
}

void APharahFPSController::JetPackAbility()
{
    UE_LOG(LogTemp, Warning, TEXT(" triggered "));
    APlayerController* CharacterController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    float InputKeyDown = CharacterController->GetInputKeyTimeDown(EKeys::SpaceBar);
    if( InputKeyDown > 0)
    {
        if( FuelAmount <= MaxFuelAmount )
        {
            
            GetMovementComponent()->Velocity.Z += 140;
            
        }
    }

    float bJetpackIsPressed =  CharacterController->GetInputAnalogKeyState(EKeys::SpaceBar);

    if(bJetpackIsPressed)
    {
        if( FuelAmount > 0)
        {
            FuelAmount -= 60;
        }
    }
    else
    {
        if ( FuelAmount <= MaxFuelAmount)
        {
            FuelAmount += 1;
        }
    }
}
