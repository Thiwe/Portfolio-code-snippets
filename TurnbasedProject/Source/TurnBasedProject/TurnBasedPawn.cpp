// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedPawn.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include <Components/SphereComponent.h>
#include "GameFramework/SpringArmComponent.h"



// Sets default values
ATurnBasedPawn::ATurnBasedPawn()
{
	bReplicates = true;


	// Create the movement component
	MovementComponent = CreateDefaultSubobject<UMovementComponent>(TEXT("MovementComponent"));

	// Set size for player sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(100.f);
	SphereComponent->SetLineThickness(20.0f);
	SphereComponent->SetCollisionProfileName(TEXT("NoCollision"));
	//set a sphere as a visual representation of the pawn
	SphereComponent->SetVisibility(true);
	SphereComponent->SetHiddenInGame(false);

	//add a arrow component to show the direction of the pawn
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ArrowComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ArrowComponent->ArrowColor = FColor::Red;
	ArrowComponent->ArrowSize = 100.0f;
	ArrowComponent->SetVisibility(true);


	// Don't rotate character to camera direction
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 25000.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, -90.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->bUsePawnControlRotation = true;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ATurnBasedPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurnBasedPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATurnBasedPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

