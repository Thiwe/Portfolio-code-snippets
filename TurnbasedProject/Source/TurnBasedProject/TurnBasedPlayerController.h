// Copyright (c) 2024 Your Company Name. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "blueprint/UserWidget.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "TurnBasedPlayerController.generated.h"

// Forward declarations
class ATurnBasedPawn;
class UInputMappingContext;
class UInputAction;
class TurnBasedGameMode;
class UUserWidget;
class UNiagaraSystem;
class ATurnBasedUnitActor;

/** Delegate for broadcasting turn state changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMyTurnChanged, bool, bMyTurn);

/**
 * Player controller class for turn-based gameplay.
 * Handles input processing, camera control, and turn management.
 */
UCLASS()
class TURNBASEDPROJECT_API ATurnBasedPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATurnBasedPlayerController();

    //~ Begin AActor Interface
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //~ End AActor Interface

    /** Sets the player's turn state and broadcasts the change */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetMyTurn(bool bInMyTurn);

    /** Sets the controller's index for player identification */
    void SetPlayerControllerIndex(int32 ThePlayerIndex);

    /** Time threshold for distinguishing between short and long presses */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    float ShortPressThreshold;

    /** Visual effect spawned on click interactions */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UNiagaraSystem* FXCursor;

    //~ Begin Input Mapping Context
    /** Primary input mapping context for the controller */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* TurnBasedMappingContext;

    /** Input action mappings */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OnMouseLeftClickAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OnMouseDoubleRightClickAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OnMouseRightClickAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OnMouseRightReleaseAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OnMouseMiddleClickAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* OnSpacebarAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CameraMoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CameraMoveUpAndDownAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CameraLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CameraZoomAction;
    //~ End Input Mapping Context

    /** Current turn state */
    UPROPERTY(BlueprintReadWrite, Category = "Game")
    bool bMyTurn;

    //~ Begin Network Functions
    UFUNCTION(Client, Reliable)
    void Client_StartTurn();

    UFUNCTION(Client, Reliable)
    void Client_EndTurn();

    UFUNCTION(Server, Reliable)
    void Server_ActionEndTurn();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_OnDragMoveAction(ATurnBasedUnitActor* HitActor);
    //~ End Network Functions

protected:
    //~ Begin AActor Interface
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    //~ End AActor Interface

    /** Centers the mouse cursor in the viewport */
    void SetMouseCursorToMiddle();

    //~ Begin Input Handlers
    void HandleSpacebarTrigger(const FInputActionValue& Value);
    void HandleLeftMouseButtonTrigger(const FInputActionValue& Value);
    void HandleDoubleRightMouseButtonTrigger(const FInputActionValue& Value);
    void HandleRightMouseButtonTrigger(const FInputActionValue& Value);
    void HandleRightMouseButtonRelease(const FInputActionValue& Value);
    void HandleMiddleMouseButtonTrigger(const FInputActionValue& Value);
    void HandleMiddleMouseButtonRelease(const FInputActionValue& Value);
    void HandleCameraMove(const FInputActionValue& Value);
    void HandleCameraMoveUpAndDown(const FInputActionValue& Value);
    void HandleCameraLook(const FInputActionValue& Value);
    void HandleCameraZoom(const FInputActionValue& Value);
    //~ End Input Handlers



private:
    /** UI widget class to spawn */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> UserWidgetClass;

    /** Camera movement parameters */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float DragSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float MovementSpeed = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float UpMoveSpeed = 100.0f;

    /** Turn state change delegate */
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnMyTurnChanged OnMyTurnChanged;

    /** Input tracking variables */
    float FollowTime;
    FVector InitialCameraLocation;
    FVector WorldHitLocation;
    FVector PreviousWorldHitLocation;
    FVector2D mouseScreenLocationGrab;
    FVector2D InitialMousePosition;
    FVector2D CurrentMousePosition;

    /** State flags */
    bool bRightMouseButtonIsPressed;
    bool bIsDragging;
    bool bCanTurnCamera;
    bool bLeftMouseButtonIsDown;

    /** References to controlled actors */
    APawn* ControlledPawn;
    ATurnBasedPawn* TurnBasedPawn;
    ATurnBasedUnitActor* SelectedUnit;

    /** Player identification */
    UPROPERTY(Replicated)
    int32 PlayerControllerIndex;

    /** Helper functions */
    void HandleRightMouseButton(const FInputActionValue& Value);
    void HandleLeftMouseButton(const FInputActionValue& Value);
    void HandleDoubleRightMouseButton(const FInputActionValue& Value);
    void HandleMiddleMouseButton(const FInputActionValue& Value);
    void HandleDragCamera();
    void SetControlledPawnLocation(FVector HitLocation);

    FORCEINLINE int32 GetPlayerControllerIndex() const { return PlayerControllerIndex; }
};