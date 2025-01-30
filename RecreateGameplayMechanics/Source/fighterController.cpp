// Fill out your copyright notice in the Description page of Project Settings.


#include "fighterController.h"

#include "FPSController.h"
#include "ISocketManager.h"

// Sets default values
AfighterController::AfighterController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Engine Varaiables
	LinearDampening = 0.5;
	AngularDampening = 4;
	ThrusterPower = 400.0;
	TorquePower = 100.0;
	BoostMultiplier = 5.0;

	//spawn point
	NrOfThrusters = 3;
	
	shipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	shipMesh->SetupAttachment(RootComponent);
	shipMesh->SetEnableGravity(false);
	shipMesh->SetLinearDamping(LinearDampening);
	shipMesh->SetAngularDamping(AngularDampening);
	shipMesh->SetSimulatePhysics(true);
		
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->TargetArmLength = 800;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritRoll = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->SetRelativeLocation(FVector(0.0, 0.0, 200.0));

	FighterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FighterCamera"));
	FighterCamera->SetupAttachment(CameraBoom);
	FighterCamera->SetRelativeLocation(FVector(0.0, 0.0, 0.0));
	FighterCamera->SetRelativeRotation(FRotator(0.0, 30.0, 0.0));

}

// Called when the game starts or when spawned
void AfighterController::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AfighterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AfighterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AfighterController::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AfighterController::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AfighterController::MoveUp);
	
	PlayerInputComponent->BindAxis("Pitch", this, &AfighterController::MovePitch);
	PlayerInputComponent->BindAxis("Jaw", this, &AfighterController::MoveYaw);
	PlayerInputComponent->BindAxis("Roll", this, &AfighterController::MoveRoll);
	
	PlayerInputComponent->BindAxis("ZoomCamera", this, &AfighterController::ZoomCamera);

	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &AfighterController::BoostPressed);
	PlayerInputComponent->BindAction("Boost", IE_Released, this, &AfighterController::BoostReleased);

	PlayerInputComponent->BindAction("LookAround",IE_Pressed, this,  &AfighterController::LookAroundPressed);
	PlayerInputComponent->BindAction("LookAround",IE_Released, this,  &AfighterController::LookAroundReleased);
	
	PlayerInputComponent->BindAction("ChangeCamera",IE_Pressed, this,  &AfighterController::ChangeCamera);

	
	
}

void AfighterController::ZoomCamera(float input)
{
	if(freeLook)
	{
		CameraBoom->TargetArmLength -= (CameraBoom->TargetArmLength) / 10 * input; 
	}
}


void AfighterController::ChangeCamera()
{
	DefaultCameraBoomarmLength = 5000;//CameraBoom->TargetArmLength;
	localBool = !localBool;
	if(localBool)
	{
		CameraBoom->TargetArmLength = 800;
	}
	else
	{
		CameraBoom->TargetArmLength = DefaultCameraBoomarmLength;
	}
}


void AfighterController::LookAroundPressed()
{
	freeLook = true;
	DefaultCameraBoomRotation = CameraBoom->GetRelativeRotation();

}

void AfighterController::LookAroundReleased()
{
	freeLook = false;
	CameraBoom->SetRelativeRotation(DefaultCameraBoomRotation);
}


void AfighterController::UpdateSideThrusters(UStaticMeshComponent* ThrusterMesh)
{
	UMaterialInstanceDynamic* MaterialInstance = ThrusterMesh->CreateDynamicMaterialInstance(0);
	FTransform const ThrusterMeshWorldTransform = ThrusterMesh->GetComponentTransform();
	FVector const ThrusterPointVelocity = shipMesh->GetPhysicsLinearVelocityAtPoint(ThrusterMeshWorldTransform.GetLocation());

	FVector const ForwardComponent = ThrusterPointVelocity.ProjectOnTo(ThrusterMesh->GetForwardVector());

	FVector const LocalForwardCompVector = ThrusterMeshWorldTransform.InverseTransformVector(ForwardComponent);

	float const LocalForwardVectorLength = LocalForwardCompVector.Size(); //FVector::Size(LocalForwardVector) //sqrt(FMath::Square(LocalForwardVector.X), FMath::Square(LocalForwardVector.Y), FMath::Square(LocalForwardVector.Z));

	FName const ScalarParam = FName("emissiveStrenght");

	if(LocalForwardCompVector.X < 0)
	{
		MaterialInstance->SetScalarParameterValue(ScalarParam, LocalForwardVectorLength/1000);
	}
	
}

void AfighterController::BoostPressed()
{
	ThrusterPower = ThrusterPower * BoostMultiplier;
}

void AfighterController::BoostReleased()
{
	ThrusterPower = ThrusterPower / BoostMultiplier;
}


void AfighterController::MoveForward(float input)
{
	//const FRotator Rotation = GetControlRotation();
	FVector ForwardVector = GetActorForwardVector();

	
	shipMesh->AddForce(ForwardVector * input * ThrusterPower, NAME_None, true);
	
}

void AfighterController::MoveRight(float input)
{
	//const FRotator Rotation = GetControlRotation();
	const FVector RightVector = GetActorRightVector();

	shipMesh->AddForce(RightVector * input * ThrusterPower, NAME_None, true);
}

void AfighterController::MoveUp(float input)
{
	const FVector UpVector = GetActorUpVector();
	
	shipMesh->AddForce(UpVector * input * ThrusterPower, NAME_None, true);
}

void AfighterController::MovePitch(float input)
{
	if (freeLook)
	{
		CameraBoom->AddRelativeRotation(FRotator(input, 0.0, 0.0));
	}
	else
	{
		const FVector RightVector = GetActorRightVector();
	    shipMesh->AddTorqueInRadians(RightVector * input * TorquePower, NAME_None, true);
	}

}
void AfighterController::MoveRoll(float input)
{
	const FVector ForwardVector = GetActorForwardVector();

	shipMesh->AddTorqueInRadians(ForwardVector * input * TorquePower, NAME_None, true);
}
void AfighterController::MoveYaw(float input)
{
	if (freeLook)
	{
		CameraBoom->AddRelativeRotation(FRotator(0.0, input, 0.0));
	}
	else
	{
		const FVector UpVector = GetActorUpVector();
		shipMesh->AddTorqueInRadians(UpVector * input * TorquePower, NAME_None, true);
	}
}


