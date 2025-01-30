// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretController.h"

#include "DrawDebugHelpers.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "macros.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"

// Sets default values
ATurretController::ATurretController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretLineOfSightRadius = 20000.0;
	MaxVerticalRotation = -60; 
	MaxHorizontalRotation =-90;
	//ProjectileClass = nullptr;         
	RotationSpeed = 3;         
	RateOfFire = 0.8;            
	CurrentTarget = nullptr;           
	IdleRotationSpeed = 1;     
	Spread = 50;
	DeltaTimeVar = 0;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Scene;

	TurretRotation = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRotation"));
	TurretRotation->SetupAttachment(RootComponent);

	MuzzleFlashLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleFlashLocation"));
	MuzzleFlashLocation->SetupAttachment(TurretRotation);
	FX_MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FX_MuzzleFlash"));
	FX_MuzzleFlash->SetupAttachment(MuzzleFlashLocation);
	FX_MuzzleFlash->SetAutoActivate(false);

	ProjectileClass = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SpawnedProjectileEffect"));
	ProjectileClass->SetupAttachment(MuzzleFlashLocation);
	ProjectileClass->SetAutoActivate(false);
	
	LineOfSightPivot = CreateDefaultSubobject<USceneComponent>(TEXT("LineOfSightPivot"));
	LineOfSightPivot->SetupAttachment(RootComponent);
	LineOfSightPivot->SetRelativeLocation(FVector::ZeroVector);
	
	TurretLineOfSight = CreateDefaultSubobject<USphereComponent>(TEXT("TurretLineOfSight"));
	TurretLineOfSight->SetupAttachment(LineOfSightPivot);
	TurretLineOfSight->SetSphereRadius(TurretLineOfSightRadius);
	TurretLineOfSight->SetRelativeLocation(FVector(0.0, 0.0, 0.0));
	TurretLineOfSight->SetCollisionResponseToAllChannels(ECR_Overlap);
	TurretLineOfSight->SetGenerateOverlapEvents(true);
	TurretLineOfSight->OnComponentBeginOverlap.AddDynamic(this, &ATurretController::OnLineOfSightBeginOverlap);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComponent);
	
	

}


// Called when the game starts or when spawned
void ATurretController::BeginPlay()
{
	Super::BeginPlay();

	if (FCurve)
	{
		ProgressFunction.BindUFunction(this, FName(TEXT("HandleTimelineUpdate")));
		/*In order to bind the function to our ufunction we need to create a FName which contains
		the name of the function we want to call every time the timeline advances.
		Binding the HandleProgress function to the ProgressFunction...*/
		MyTimeline.SetLooping(true);
		//ProgressFunction.BindUFunction(this, FName("HandleTimelineUpdate"));
		//Setting up the loop status and the function that is going to fire when the timeline ticks
		MyTimeline.AddInterpFloat(FCurve, ProgressFunction, FName("Alpha"));

		MyTimeline.PlayFromStart();



	}
	
}

// Called every frame
void ATurretController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTimeVar = DeltaTime;
	if(nullptr == CurrentTarget)
	{
		FRotator CurrentTargetRotation(0.0, 0.0, 0.0);


		//Advancing the timeline in every tick
		MyTimeline.TickTimeline(DeltaTime);

		FRotator MaxHorizontalRotationVar(0.0, MaxHorizontalRotation, 0.0);
		FRotator MaxHorizontalRotationAbsVar(0.0, std::abs(MaxHorizontalRotation), 0.0);
		FRotator LerpRotation(FMath::Lerp(MaxHorizontalRotationVar, MaxHorizontalRotationAbsVar, TimelineAlphaOut)); // todo dublecheck timelinealphaout

		FRotator InterpRotator(FMath::RInterpTo(TurretRotation->GetRelativeRotation(), LerpRotation, DeltaTimeVar, IdleRotationSpeed));

		
		TurretRotation->SetRelativeRotation(InterpRotator);

	}
	else
	{
		if (GetTurretTargetComponent(CurrentTarget)->IsDead)
		{
			OnLostTarget();
			CurrentTarget = nullptr;
			if (nullptr != FindFirstTarget())
			{
				CurrentTarget = FindFirstTarget();
				OnHasTarget();
			}
		}
		else
		{
			FRotator CurrentTargetWorldRotation = TurretRotation->GetComponentRotation();//K2_GetComponentRotation();
			FRotator TurretToTargetRotation = UKismetMathLibrary::FindLookAtRotation(TurretRotation->GetComponentLocation(),
														   GetTurretTargetComponent(CurrentTarget)->GetComponentLocation());
			FRotator NewTurretRotation(FMath::RInterpTo(CurrentTargetWorldRotation, TurretToTargetRotation, DeltaTime, RotationSpeed));
			TurretRotation->SetWorldRotation(NewTurretRotation);

			FRotator TurretRelativeRotation = TurretRotation->GetRelativeRotation();
			float VerticalRotationClamped = FMath::Clamp(TurretRelativeRotation.Pitch, MaxVerticalRotation, std::abs(MaxVerticalRotation));
			float HorizontalRotationClamped = FMath::Clamp(TurretRelativeRotation.Yaw, MaxHorizontalRotation, std::abs(MaxHorizontalRotation));
			FRotator NewRotation(TurretRelativeRotation.Pitch, TurretRelativeRotation.Yaw, 0.0);
			TurretRotation->SetRelativeRotation(NewRotation);

		}
	}

}


void ATurretController::HandleTimelineUpdate(float Value)
{
	TimelineAlphaOut = Value;
}

void ATurretController::OnLostTarget()
{
	FX_MuzzleFlash->Deactivate();
	//Starting the timeline...
	MyTimeline.PlayFromStart();
	
}

void ATurretController::OnHasTarget()
{
	MyTimeline.Stop();
}

UTurretTargetComponent* ATurretController::GetTurretTargetComponent(AActor* CurrentTargetActor)
{
	UTurretTargetComponent* Target = nullptr;
	TArray<UTurretTargetComponent*> TargetArray;
	TargetArray.Add(CurrentTargetActor->FindComponentByClass<UTurretTargetComponent>());
	
	for (UTurretTargetComponent* TargetComp : TargetArray)
	{
		if(TargetComp)
		{
			Target = TargetComp;
		}
	}
	return Target;
}

void ATurretController::Fire()
{
	AActor* MyOwner = GetOwner();
	if(CurrentTarget != nullptr)
	{

		FTransform MuzzleFlashWorldTransform(MuzzleFlashLocation->GetComponentTransform());
		FVector MuzzleFlashWorldLocation(MuzzleFlashWorldTransform.GetLocation());
		FRotator SpreadRandomNr(UKismetMathLibrary::RandomFloatInRange(Spread*(-1), Spread),
								UKismetMathLibrary::RandomFloatInRange(Spread*(-1), Spread),
								UKismetMathLibrary::RandomFloatInRange(Spread*(-1), Spread));
		FRotator MuzzleFlashSWithSpreadRotation(UKismetMathLibrary::ComposeRotators(MuzzleFlashLocation->GetComponentRotation(), SpreadRandomNr));

		FQuat MuzzleFlashSWithSpreadQuat( MuzzleFlashWorldTransform.GetRotation() + SpreadRandomNr.Quaternion());

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		
		FTransform ProjectileSpawnTransform(MuzzleFlashSWithSpreadRotation, MuzzleFlashWorldLocation, MuzzleFlashWorldTransform.GetScale3D()*10);
		if(ProjectileClass || SystemTemplate)
		{
			FX_MuzzleFlash->Activate(true);
			//UParticleSystemComponent* SpawnedParticle = GetWorld()->SpawnActor<UParticleSystemComponent>(ProjectileClass->GetClass(), ProjectileSpawnTransform, SpawnParams);
			UNiagaraComponent* spawnedNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemTemplate, MuzzleFlashLocation->GetComponentLocation(), MuzzleFlashLocation->GetComponentRotation());
			FHitResult Hit;
			//DrawDebugLine(GetWorld(), MuzzleFlashWorldLocation, MuzzleFlashWorldLocation+MuzzleFlashLocation->GetForwardVector()*10000, FColor::Red,false, 1,0, 50);

			FVector lineTraceEndPoint = MuzzleFlashWorldLocation+MuzzleFlashLocation->GetForwardVector()*TurretLineOfSightRadius;
			
			if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleFlashWorldLocation, lineTraceEndPoint, ECollisionChannel::ECC_WorldDynamic, QueryParams))
			{
				
				ProjectileBeamEnd = Hit.ImpactPoint;
				
				FVector HitResultLocal = UKismetMathLibrary::InverseTransformLocation(MuzzleFlashLocation->GetComponentTransform(), ProjectileBeamEnd);
				spawnedNiagaraComp->SetVectorParameter(FName("BeamEndPoint"), HitResultLocal);
				 DrawDebugSphere(GetWorld(), Hit.Location, 100, 20, FColor::Red, false, 4, 0, 20);
				// DrawDebugSphere(GetWorld(), HitResultLocal, 100, 20, FColor::Green, false, 4, 0, 20);
				// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, ProjectileBeamEnd.ToString());
				// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Hit.Location.ToString());
				// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, HitResultLocal.ToString());
				
				//spawnedNiagaraComp->SetVectorParameter(FName("ProjectileVelocity"), spawnedNiagaraComp->GetForwardVector()*100);
				//ProjectileClass->Activate(true);
				FVector ShotDirection = ProjectileBeamEnd - MuzzleFlashWorldLocation;
				UGameplayStatics::SpawnEmitterAtLocation( 	GetWorld(), 
															ImpactEffect, 
															ProjectileBeamEnd, 
															ShotDirection.Rotation() );
			}
			// if(spawnedProjectile)
   //          {
   //          	
   //          	spawnedProjectile->Activate(true);
   //          	spawnedProjectile->AddForce(GetActorForwardVector()*1, NAME_None, true);
   //          	FX_MuzzleFlash->Activate(true);
   //          }
		}
		
		

	}
}

AActor* ATurretController::FindFirstTarget()
{
	AActor* Target = nullptr;
	TArray<AActor*> OverlappingActors;
	TSubclassOf<AActor> classFilter; 
	TurretLineOfSight->GetOverlappingActors(OverlappingActors, classFilter);

	for (AActor* TargetActor : OverlappingActors)
	{
		UTurretTargetComponent* TurretTargetComponent = GetTurretTargetComponent (TargetActor);
		if(TurretTargetComponent != nullptr)
		{
			if(false == TurretTargetComponent->IsDead)
			{
				Target = TargetActor;
			}
		}
	}
	return Target;
}

void ATurretController::OnLineOfSightBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(GetTurretTargetComponent(OtherActor))
	{
		if(CurrentTarget == nullptr)
		{
			CurrentTarget = OtherActor;
			OnHasTarget();
			GetWorldTimerManager().SetTimer(FireDelay,this, &ATurretController::Fire, RateOfFire,true, 0.5);
		}
	}
}


