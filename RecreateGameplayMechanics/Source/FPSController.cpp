// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSController.h"

// Sets default values
AFPSController::AFPSController()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = -10;

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(CameraBoom);
	FPSCamera->SetRelativeLocation(FVector(0.0, 0.0, 70.0));

	
	

}

// Called when the game starts or when spawned
void AFPSController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSController::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSController::MoveRight);
	
	PlayerInputComponent->BindAxis("Pitch", this, &AFPSController::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Jaw", this, &AFPSController::AddControllerYawInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

}

void AFPSController::MoveForward(float input)
{
	
	// Find out which way is "forward" and record that the player wants to move that way.
	const FRotator Rotation = GetControlRotation();
	//const FVector ZDirection = FVector(0.0, 0.0, Direction.Z);
	FRotator trial = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector Direction = FRotationMatrix(trial).GetScaledAxis(EAxis::X);
	
	AddMovementInput( Direction, input);
}

void AFPSController::MoveRight(float input)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	const FRotator Rotation = GetControlRotation();
	//const FVector ZDirection = FVector(0.0, 0.0, Direction.Z);
	FRotator trial = FRotator(0.0, Rotation.Yaw, 0.0);
	const FVector Direction = FRotationMatrix(trial).GetScaledAxis(EAxis::Y);
	
	AddMovementInput( Direction, input);
	
}

