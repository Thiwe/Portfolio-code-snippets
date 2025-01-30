// Fill out your copyright notice in the Description page of Project Settings.


// Copyright (c) 2024 Your Company Name. All rights reserved.

#include "TurnBasedPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "TurnBasedGameMode.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/MovementComponent.h"
#include "InputCoreTypes.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "TurnBasedPawn.h"
#include "Components/PrimitiveComponent.h"
#include "TurnBasedUnitActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

ATurnBasedPlayerController::ATurnBasedPlayerController()
{
    // Initialize default values
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bReplicates = true;
    DefaultMouseCursor = EMouseCursor::Default;

    // Initialize state flags
    bRightMouseButtonIsPressed = false;
    bIsDragging = false;
    bCanTurnCamera = false;
    bLeftMouseButtonIsDown = false;
}

void ATurnBasedPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Only create UI for local player
    if (IsLocalPlayerController())
    {
        ControlledPawn = GetPawn();
        TurnBasedPawn = Cast<ATurnBasedPawn>(ControlledPawn);

        // Create and add user widget to viewport
        if (UserWidgetClass)
        {
            if (UUserWidget* UserWidgetInstance = CreateWidget<UUserWidget>(this, UserWidgetClass))
            {
                UserWidgetInstance->AddToViewport();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UserWidgetClass is not set in the PlayerController!"));
        }
    }
}

void ATurnBasedPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Handle continuous actions
    if (bIsDragging)
    {
        HandleDragCamera();
    }

    if (bLeftMouseButtonIsDown)
    {
        Server_OnDragMoveAction_Implementation(SelectedUnit);
    }
}

void ATurnBasedPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Set up Enhanced Input system
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(TurnBasedMappingContext, 0);
    }

    // Bind input actions
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Basic actions
        EnhancedInputComponent->BindAction(OnSpacebarAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleSpacebarTrigger);

        // Mouse actions
        EnhancedInputComponent->BindAction(OnMouseLeftClickAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleLeftMouseButtonTrigger);
        EnhancedInputComponent->BindAction(OnMouseDoubleRightClickAction, ETriggerEvent::Completed,
            this, &ATurnBasedPlayerController::HandleDoubleRightMouseButtonTrigger);
        EnhancedInputComponent->BindAction(OnMouseRightClickAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleRightMouseButtonTrigger);
        EnhancedInputComponent->BindAction(OnMouseRightReleaseAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleRightMouseButtonRelease);
        EnhancedInputComponent->BindAction(OnMouseMiddleClickAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleMiddleMouseButtonTrigger);
        EnhancedInputComponent->BindAction(OnMouseMiddleClickAction, ETriggerEvent::Completed,
            this, &ATurnBasedPlayerController::HandleMiddleMouseButtonRelease);

        // Camera controls
        EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleCameraMove);
        EnhancedInputComponent->BindAction(CameraMoveUpAndDownAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleCameraMoveUpAndDown);
        EnhancedInputComponent->BindAction(CameraLookAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleCameraLook);
        EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered,
            this, &ATurnBasedPlayerController::HandleCameraZoom);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Enhanced Input Component not found."));
    }
}

//~ Begin Network Functions
void ATurnBasedPlayerController::Client_StartTurn_Implementation()
{
    SetMyTurn(true);
}

void ATurnBasedPlayerController::Client_EndTurn_Implementation()
{
    SetMyTurn(false);
}

void ATurnBasedPlayerController::Server_ActionEndTurn_Implementation()
{
    // Get the game mode and cycle turns
    if (ATurnBasedGameMode* GameMode = Cast<ATurnBasedGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->CycleTurns();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode not found."));
    }
}

void ATurnBasedPlayerController::Server_OnDragMoveAction_Implementation(ATurnBasedUnitActor* TurnBasedUnitActor)
{
    if (!TurnBasedUnitActor)
    {
        return;
    }

    // Set up trace parameters
    FHitResult HitResult;
    FVector Start, Dir, End;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    QueryParams.AddIgnoredActor(TurnBasedUnitActor);

    // Perform trace to find new location
    DeprojectMousePositionToWorld(Start, Dir);
    End = Start + (Dir * 10000.0f);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
    {
        if (USceneComponent* PickedComponent = TurnBasedUnitActor->GetRootComponent())
        {
            PickedComponent->SetWorldLocation(HitResult.Location);
        }
    }
}

bool ATurnBasedPlayerController::Server_OnDragMoveAction_Validate(ATurnBasedUnitActor* TurnBasedUnitActor)
{
    return true; // Add validation logic if needed
}
//~ End Network Functions

//~ Begin Input Handlers
void ATurnBasedPlayerController::HandleLeftMouseButtonTrigger(const FInputActionValue& Value)
{
    if (!bMyTurn)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Mouse Clicked"));

    // Perform trace under cursor
    FHitResult HitResult;
    GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

    if (AActor* HitActor = HitResult.GetActor())
    {
        if (ATurnBasedUnitActor* Unit = Cast<ATurnBasedUnitActor>(HitActor))
        {
            if (Unit->GetUnitActorIndex() == PlayerControllerIndex)
            {
                SelectedUnit = Unit;
                bLeftMouseButtonIsDown = !bLeftMouseButtonIsDown;
            }
        }
    }
}

void ATurnBasedPlayerController::HandleSpacebarTrigger(const FInputActionValue& Value)
{
    if (bMyTurn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ending Turn"));
        Server_ActionEndTurn();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not your turn."));
    }
}

void ATurnBasedPlayerController::HandleDoubleRightMouseButtonTrigger(const FInputActionValue& Value)
{
    // Delay location setting to ensure proper registration
    HandleDoubleRightMouseButton(Value);
}

void ATurnBasedPlayerController::HandleRightMouseButtonTrigger(const FInputActionValue& Value)
{
    bIsDragging = true;
    GetMousePosition(InitialMousePosition.X, InitialMousePosition.Y);
    UE_LOG(LogTemp, Warning, TEXT("Right mouse button pressed %s"), *InitialMousePosition.ToString());

    if (ControlledPawn)
    {
        InitialCameraLocation = ControlledPawn->GetActorLocation();
    }
}

void ATurnBasedPlayerController::HandleRightMouseButtonRelease(const FInputActionValue& Value)
{
    bRightMouseButtonIsPressed = false;
    bIsDragging = false;
    UE_LOG(LogTemp, Warning, TEXT("Right mouse button released"));
}

void ATurnBasedPlayerController::HandleMiddleMouseButtonTrigger(const FInputActionValue& Value)
{
    bShowMouseCursor = false;
    bCanTurnCamera = true;
}

void ATurnBasedPlayerController::HandleMiddleMouseButtonRelease(const FInputActionValue& Value)
{
    bShowMouseCursor = true;
    bCanTurnCamera = false;
}
//~ End Input Handlers

//~ Begin Camera Control Functions
void ATurnBasedPlayerController::HandleCameraMoveUpAndDown(const FInputActionValue& Value)
{
    if (!ControlledPawn)
    {
        return;
    }

    float MovementVector = Value.Get<float>();
    TurnBasedPawn->AddActorWorldOffset(FVector(0.f, 0.f, MovementVector) * UpMoveSpeed);
}

void ATurnBasedPlayerController::HandleCameraMove(const FInputActionValue& Value)
{
    if (!ControlledPawn)
    {
        return;
    }

    FVector MovementVector = Value.Get<FVector>();

    // Calculate movement vectors
    FVector RightVector = ControlledPawn->GetActorForwardVector();
    FVector ForwardVector = ControlledPawn->GetActorRightVector();

    // Ensure horizontal movement only
    ForwardVector.Z = 0.0f;
    RightVector.Z = 0.0f;

    // Normalize vectors
    ForwardVector.Normalize();
    RightVector.Normalize();

    // Calculate and apply movement
    FVector WorldDelta = (ForwardVector * MovementVector.Y + RightVector * MovementVector.X) * MovementSpeed;
    TurnBasedPawn->AddActorWorldOffset(WorldDelta);
}

void ATurnBasedPlayerController::HandleDragCamera()
{
    GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
    FVector2D MouseDelta = CurrentMousePosition - InitialMousePosition;

    if (ControlledPawn)
    {
        // Calculate movement vectors
        FVector ForwardVector = ControlledPawn->GetActorForwardVector();
        FVector RightVector = -ControlledPawn->GetActorRightVector();

        // Ensure horizontal movement only
        ForwardVector.Z = 0.0f;
        RightVector.Z = 0.0f;

        // Normalize vectors
        ForwardVector.Normalize();
        RightVector.Normalize();

        // Calculate and apply movement
        FVector WorldDelta = (ForwardVector * MouseDelta.Y + RightVector * MouseDelta.X) * DragSpeed;
        FVector NewLocation = InitialCameraLocation + WorldDelta;
        NewLocation.Z = InitialCameraLocation.Z; // Maintain height

        ControlledPawn->SetActorLocation(NewLocation, false);
    }
}

void ATurnBasedPlayerController::HandleCameraZoom(const FInputActionValue& Value)
{
    if (!TurnBasedPawn)
    {
        return;
    }

    if (USpringArmComponent* CameraBoom = TurnBasedPawn->GetCameraBoom())
    {
        float TargetArmLength = CameraBoom->TargetArmLength;
        float InputValue = Value.Get<float>() * 1000.f;
        CameraBoom->TargetArmLength = FMath::Clamp(TargetArmLength + InputValue, 0.0f, 10000.0f);
    }
}

void ATurnBasedPlayerController::HandleCameraLook(const FInputActionValue& Value)
{
    if (!bCanTurnCamera || !ControlledPawn)
    {
        return;
    }

    FVector2D LookAxisVector = Value.Get<FVector2D>();

    ControlledPawn->AddControllerYawInput(LookAxisVector.X);
    ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);

    SetMouseCursorToMiddle();
}
//~ End Camera Control Functions

//~ Begin Helper Functions
void ATurnBasedPlayerController::HandleDoubleRightMouseButton(const FInputActionValue& Value)
{
    FHitResult HitResult;
    GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

    if (ControlledPawn)
    {
        GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
        FVector HitLocation = HitResult.Location;

        // Set a short delay before updating location
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            [this, HitLocation]() { SetControlledPawnLocation(HitLocation); },
            0.1f,
            false
        );
    }
}

void ATurnBasedPlayerController::SetControlledPawnLocation(FVector HitLocation)
{
    if (ControlledPawn)
    {
        ControlledPawn->SetActorLocation(HitLocation);
    }
}

void ATurnBasedPlayerController::SetPlayerControllerIndex(int32 ThePlayerIndex)
{
    PlayerControllerIndex = ThePlayerIndex;
}

void ATurnBasedPlayerController::SetMouseCursorToMiddle()
{
    int32 ViewportSizeX, ViewportSizeY;
    GetViewportSize(ViewportSizeX, ViewportSizeY);
    SetMouseLocation(ViewportSizeX / 2, ViewportSizeY / 2);
}

void ATurnBasedPlayerController::SetMyTurn(bool bInMyTurn)
{
    bMyTurn = bInMyTurn;
    OnMyTurnChanged.Broadcast(bMyTurn);
}
//~ End Helper Functions

void ATurnBasedPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATurnBasedPlayerController, PlayerControllerIndex);
}
