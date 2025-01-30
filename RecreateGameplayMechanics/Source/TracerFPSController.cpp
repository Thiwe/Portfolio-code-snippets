// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsCreateFPS/Public/TracerFPSController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Controller.h"


// Sets default values
ATracerFPSController::ATracerFPSController()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    TimeBeforeSettingNewPos = 0.6;
    bCanSetPos = true;
    bCanRecall = true;
    bRecallPressed = false;
    Counter = 0;
    RecallSpeed = 5;
}

void ATracerFPSController::SetTracerPosition()
{
    if(RecallPositions.Num() >= 8)
    {
        FTransform lastIndex = RecallPositions.Last();
        RecallPositions.RemoveAt(0);
        UE_LOG(LogTemp, Warning, TEXT("amount in array %i"), RecallPositions.Num());
    }
    else
    {
        RecallPositions.Add(GetActorTransform());

        const FTransform LastIndex = RecallPositions.Top();
        const FMatrix TransformThing = FMatrix(LastIndex.ToMatrixNoScale());
        UE_LOG(LogTemp, Warning, TEXT("amount in array %i"), RecallPositions.Num());
        DrawDebugFrustum(GetWorld(), TransformThing, FColor::Blue, false, 10, 0, 5.0 );
    }
}



// Called when the game starts or when spawned
void ATracerFPSController::BeginPlay()
{
    Super::BeginPlay();

    if (bCanSetPos)
    {
        GetWorldTimerManager().SetTimer(SetPosTimerHandle, this, &ATracerFPSController::SetTracerPosition, TimeBeforeSettingNewPos, true);
    }
}

// Called every frame
void ATracerFPSController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(bRecallPressed)
    {
        const float AlphaValue = (UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) * RecallSpeed);
        const FTransform ActorTransform = GetActorTransform();
        const FTransform NewTransform = UKismetMathLibrary::TLerp(ActorTransform, RecallPositions[Counter], AlphaValue);
        SetActorLocation(NewTransform.GetLocation());

        const FRotator& ActorRotation = FRotator( ActorTransform.GetRotation());
        const FRotator NewRotation = UKismetMathLibrary::RLerp(ActorRotation, NewTransform.GetRotation().Rotator(), AlphaValue, false);

        AController* OwningController = GetController();
        if(OwningController != nullptr)
        {
            OwningController->SetControlRotation(NewRotation);
        }
        if( UKismetMathLibrary::NearlyEqual_TransformTransform( ActorTransform, RecallPositions[Counter], 120.0, 120.0, 120.0) )
        {
            if(Counter != 0)
            {
                Counter--;
            }
            else
            {
                bRecallPressed = false;
                RecallPositions.Empty();
                GetWorldTimerManager().SetTimer(RecallCooldownTimerHandle, this, &ATracerFPSController::ResetAbility, 6, false);
                
            }
        }
    }
}

void ATracerFPSController::ResetAbility()
{
    bCanRecall = true;
}

void ATracerFPSController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ATracerFPSController::Ability);
}

void ATracerFPSController::Ability()
{
    //recall input
    if( bCanRecall)
    {
        bRecallPressed = true;
        bCanRecall = false;
        bCanSetPos = false;
        Counter = RecallPositions.Num()-1;
    }
    UE_LOG(LogTemp, Warning, TEXT("ABILITY!!!!"));
}
