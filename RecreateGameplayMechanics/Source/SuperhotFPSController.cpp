// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperhotFPSController.h"

#include "Kismet/GameplayStatics.h"

ASuperhotFPSController::ASuperhotFPSController()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ProjectileSpawnPos = CreateDefaultSubobject<USceneComponent>(TEXT("Component"));
    ProjectileSpawnPos->SetupAttachment(RootComponent);
    ProjectileSpawnPos->SetRelativeLocation(FVector(60.0, 0.0, 40.0));
    
    HAxis = 0;
    VAxis = 0;
}

void ASuperhotFPSController::BeginPlay()
{
    Super::BeginPlay();
}

void ASuperhotFPSController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if( HAxis != 0 || VAxis != 0)
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
    }
    else
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.065);
    }
}

void ASuperhotFPSController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ASuperhotFPSController::Shoot);

    
}

void ASuperhotFPSController::Shoot()
{
    UE_LOG(LogTemp, Warning, TEXT("BANG!"));
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = nullptr;
    ASuperhotProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ASuperhotProjectile>(ProjectileClass, ProjectileSpawnPos->GetComponentLocation(), ProjectileSpawnPos->GetComponentRotation(), SpawnParams);
    const FRotator CameraRotation = AFPSController::FPSCamera->GetComponentRotation();

    
    SpawnedProjectile->MeshComp->AddImpulse(FVector(CameraRotation.Quaternion().GetForwardVector() * 18000), NAME_None, false);


}

void ASuperhotFPSController::MoveForward(float input)
{
	
    // Find out which way is "forward" and record that the player wants to move that way.
    const FRotator Rotation = GetControlRotation();
    //const FVector ZDirection = FVector(0.0, 0.0, Direction.Z);
    FRotator trial = FRotator(0.0, Rotation.Yaw, 0.0);
    const FVector Direction = FRotationMatrix(trial).GetScaledAxis(EAxis::X);
	
    AddMovementInput( Direction, input);

    VAxis = input;
}

void ASuperhotFPSController::MoveRight(float input)
{
    // Find out which way is "forward" and record that the player wants to move that way.
    const FRotator Rotation = GetControlRotation();
    //const FVector ZDirection = FVector(0.0, 0.0, Direction.Z);
    FRotator trial = FRotator(0.0, Rotation.Yaw, 0.0);
    const FVector Direction = FRotationMatrix(trial).GetScaledAxis(EAxis::Y);
	
    AddMovementInput( Direction, input);

    HAxis = input;
	
}
