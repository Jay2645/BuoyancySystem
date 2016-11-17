// Copyright 2016 Schemepunk Studios

#include "BuoyancySystemPrivatePCH.h"
#include "ShipPawn.h"


AShipPawn::AShipPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boat Mesh"));
	RootComponent = PawnMesh;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->RelativeRotation = FRotator(0.0f, 90.0f, 00.f);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->TargetOffset = FVector(0, 0, 150);
	SpringArm->bEnableCameraLag = false;
	SpringArm->AddRelativeLocation(FVector(0, 275, 0));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	/*PhysicsComponent = CreateDefaultSubobject<UPhysicsComponent>(TEXT("Physics Component"));
	PhysicsComponent->SetPrimitiveComponent(PawnMesh);
	PhysicsComponent->SetEnableGravity(false);*/

	BuoyancyComponent = CreateDefaultSubobject<UBuoyantComponent>(TEXT("Buoyancy Component"));
	BuoyancyComponent->SetPhysicsMesh(PawnMesh);


	AutoPossessPlayer = EAutoReceiveInput::Player0;

	MaxSpeed = 5.0f;
	MaxRudderAngle = 45.0f;
	AccelerationPerFrame = 0.1f;
	MaxAcceleration = 0.1f;
	TurnSpeed = 10.0f;
}

// Called when the game starts or when spawned
void AShipPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Accelerate();
}

// Called to bind functionality to input
void AShipPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveVertical", this, &AShipPawn::ChangeThrottle);
	PlayerInputComponent->BindAxis("MoveHorizontal", this, &AShipPawn::AdjustTurn);
	PlayerInputComponent->BindAxis("MousePitch", this, &AShipPawn::PitchCamera);
	PlayerInputComponent->BindAxis("MouseYaw", this, &AShipPawn::YawCamera);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UBuoyantComponent* AShipPawn::GetBuoyancyComponent_Implementation()
{
	return BuoyancyComponent;
}

/*UPhysicsComponent* AShipPawn::GetPhysicsComponent()
{
	return PhysicsComponent;
}*/

void AShipPawn::Accelerate()
{
	FTransform actorTransform = GetTransform();

	FWavePointData waveData = BuoyancyComponent->ProcessWaveHeightAtPoint(PropellerLocation, actorTransform);

	if (waveData.bIsUnder)
	{
		FVector transformedPoint = actorTransform.TransformPosition(PropellerLocation);

		FVector accelerationAmount = GetActorRightVector() * MaxSpeed * MovementInput.X;

		PawnMesh->AddImpulse(accelerationAmount, NAME_None, true);

		if (BuoyancyComponent->bDrawDebugSpheres)
		{
			DrawDebugSphere(GetWorld(), transformedPoint, BuoyancyComponent->PointThickness, 32, FColor(0, 255, 0));
			DrawDebugLine(GetWorld(), transformedPoint, transformedPoint + accelerationAmount, FColor(0, 255, 0));
		}
	}

	FVector heading = PawnMesh->GetComponentVelocity();
	if ((heading.X > 10.0f || heading.X < -10.0f) && (heading.Y > 10.0f || heading.Y < -10.0f))
	{
		float rudderTurnAmount = MaxRudderAngle * MovementInput.Y;
		FRotator rotator = GetActorRotation().Add(rudderTurnAmount, 0.0f, 0.0f);
		//FVector rudderHeading = heading.RotateAngleAxis(rudderTurnAmount, FVector::RightVector);
		FVector rudderHeading = rotator.RotateVector(heading);
		rudderHeading.X = FMath::Abs(rudderHeading.X * 0.01f);
		rudderHeading.Y = FMath::Abs(rudderHeading.Y * 0.01f);
		rudderHeading.Z = FMath::Abs(rudderHeading.Z);

		FVector combinedHeading = rudderHeading *  MovementInput.X * 0.01f;

		float dragAmount = MovementInput.Y;
		FVector headingWithDrag = combinedHeading * TurnSpeed * dragAmount;

		PawnMesh->AddTorque(headingWithDrag, NAME_None, true);
		//PawnMesh->AddForceAtLocation(headingWithDrag, actorTransform.TransformPosition(PropellerLocation));
		//PawnMesh->AddAngularImpulse(rotationVector * 0.001f, NAME_None, true);
	}
	//PawnMesh->AddAngularImpulse(FVector(0,0, TurnSpeed * MovementInput.X * MovementInput.X * MovementInput.Y),NAME_None,true);
}

void AShipPawn::ChangeThrottle(float axisValue)
{
	float value = FMath::Clamp<float>(axisValue, -1.0f, 1.0f);
	CurrentAcceleration += value * AccelerationPerFrame;
	CurrentAcceleration = FMath::Clamp<float>(CurrentAcceleration, -MaxAcceleration, MaxAcceleration);
	MovementInput.X = CurrentAcceleration / MaxAcceleration;
	if (FMath::Abs(MovementInput.X) < 0.1f)
	{
		MovementInput.X = 0.0f;
	}
}

void AShipPawn::AdjustTurn(float axisValue)
{
	MovementInput.Y = FMath::Clamp<float>(axisValue, -1.0f, 1.0f);
}

void AShipPawn::PitchCamera(float axisValue)
{
	CameraInput.Y = axisValue;
}

void AShipPawn::YawCamera(float axisValue)
{
	CameraInput.X = axisValue;
}