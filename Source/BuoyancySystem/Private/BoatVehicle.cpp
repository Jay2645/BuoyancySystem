// Fill out your copyright notice in the Description page of Project Settings.

#include "BoatVehicle.h"
#include "WheeledVehicleMovementComponent.h"

ABoatVehicle::ABoatVehicle()
{
	// Buoyancy
	BuoyantComponent = CreateDefaultSubobject<UBuoyantComponent>(TEXT("Buoyant Component"));
	// Set up buoyancy for more ship-like movement
	BuoyantComponent->PointThickness = 100.0f;
	BuoyantComponent->DisplacementRatio = 1.0f;
	BuoyantComponent->WaterTestPoints.Empty();
	for (int i = -2; i <= 2; i++)
	{
		FVector portPoint = FVector(i * 100.0f, -100.0f, 50.0f);
		FVector midPoint = FVector(i * 100.0f, 0.0f, 10.0f);
		FVector starboardPoint = FVector(i * 100.0f, 100.0f, 50.0f);
		BuoyantComponent->WaterTestPoints.Add(portPoint);
		BuoyantComponent->WaterTestPoints.Add(midPoint);
		BuoyantComponent->WaterTestPoints.Add(starboardPoint);
	}
	BuoyantComponent->WaterTestPoints.Add(FVector(-350.0f, 0.0f, 80.0f));
	BuoyantComponent->WaterTestPoints.Add(FVector(300.0f, 0.0f, 80.0f));
	BuoyantComponent->PhysicsComponent = GetMesh();
	GetMesh()->SetEnableGravity(false);

	// Camera
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	CameraArm->TargetArmLength = 1000.0f;
	CameraArm->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	CameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	CameraArm->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.0f, -20.0f, 0.0f)));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(CameraArm,FAttachmentTransformRules(EAttachmentRule::KeepRelative,false));
	Camera->SetRelativeLocationAndRotation(FVector::ZeroVector,FQuat::Identity);

	// Movement
	UWheeledVehicleMovementComponent* movementComponent = GetVehicleMovementComponent();

	// Wheels
	TArray<FWheelSetup> wheelSetups;
	BowShipWheelClass = UBowShipVehicleWheel::StaticClass();
	SternShipWheelClass = USternShipVehicleWheel::StaticClass();
	// Bow
	FWheelSetup portBowWheel;
	portBowWheel.WheelClass = BowShipWheelClass;
	portBowWheel.BoneName = TEXT("BowPortWheel");
	portBowWheel.AdditionalOffset = FVector(400.0f, -75.0f, 250.0f);
	FWheelSetup starboardBowWheel;
	starboardBowWheel.WheelClass = BowShipWheelClass;
	starboardBowWheel.BoneName = TEXT("BowStarboardWheel");
	starboardBowWheel.AdditionalOffset = FVector(400.0f, 75.0f, 250.0f);
	wheelSetups.Add(portBowWheel);
	wheelSetups.Add(starboardBowWheel);
	// Stern
	FWheelSetup portSternWheel;
	portSternWheel.WheelClass = SternShipWheelClass;
	portSternWheel.BoneName = TEXT("SternPortWheel");
	portSternWheel.AdditionalOffset = FVector(-200.0f, -75.0f, 250.0f);
	FWheelSetup starboardSternWheel;
	starboardSternWheel.WheelClass = SternShipWheelClass;
	starboardSternWheel.BoneName = TEXT("SternStarboardWheel");
	starboardSternWheel.AdditionalOffset = FVector(-200.0f, 75.0f, 250.0f);
	wheelSetups.Add(portSternWheel);
	wheelSetups.Add(starboardSternWheel);
	movementComponent->WheelSetups = wheelSetups;

	// Stencil
	WaterStencil = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water Stencil"));
	WaterStencil->CustomDepthStencilValue = 253;
	WaterStencil->bRenderCustomDepth = true;
	WaterStencil->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WaterStencil->AttachToComponent(GetMesh(),FAttachmentTransformRules(EAttachmentRule::KeepWorld,true));
	WaterStencil->SetRelativeLocation(FVector::ZeroVector);
}

void ABoatVehicle::BeginPlay()
{
	Super::BeginPlay();
	BuoyantComponent->OnStartSinkDelegate.AddDynamic(this, &ABoatVehicle::OnStartSink);
	BuoyantComponent->OnSinkDelegate.AddDynamic(this, &ABoatVehicle::OnSink);
	if (BuoyantComponent->bDrawDebugSpheres)
	{
		UWheeledVehicleMovementComponent* movementComponent = GetVehicleMovementComponent();
		if (movementComponent == NULL)
		{
			return;
		}
		movementComponent->DrawDebugLines();
	}
}

UBuoyantComponent* ABoatVehicle::GetBuoyancyComponent_Implementation()
{
	return BuoyantComponent;
}

void ABoatVehicle::RegisterShipSystem(UShipSystem* ShipSystem)
{
	ShipSystems.Add(ShipSystem);
}

void ABoatVehicle::SetThrottle(float ThrottleAmount)
{
	UWheeledVehicleMovementComponent* movementComponent = GetVehicleMovementComponent();
	if (movementComponent == NULL)
	{
		return;
	}
	movementComponent->SetThrottleInput(ThrottleAmount);

	float rpmPercent = movementComponent->GetEngineRotationSpeed() / movementComponent->GetEngineMaxRotationSpeed();
	for (int i = 0; i < ShipSystems.Num(); i++)
	{
		ShipSystems[i]->UpdateEngineRPM(rpmPercent);
	}
}

void ABoatVehicle::SetSteering(float SteeringAmount)
{
	UWheeledVehicleMovementComponent* movementComponent = GetVehicleMovementComponent();
	if (movementComponent == NULL)
	{
		return;
	}
	movementComponent->SetSteeringInput(SteeringAmount);

	for (int i = 0; i < ShipSystems.Num(); i++)
	{
		ShipSystems[i]->UpdateSteering(SteeringAmount);
	}
}

void ABoatVehicle::OnStartSink()
{
	bIsSinking = true;

	WaterStencil->bRenderCustomDepth = false;
	WaterStencil->CustomDepthStencilValue = 0;

	for (int i = 0; i < ShipSystems.Num(); i++)
	{
		ShipSystems[i]->OnStartSink();
	}
	AController* controller = GetController();
	controller->UnPossess();

	UWheeledVehicleMovementComponent* movementComponent = GetVehicleMovementComponent();
	if (movementComponent == NULL)
	{
		return;
	}
	movementComponent->DestroyPhysicsState();
	movementComponent->DestroyComponent();
}

void ABoatVehicle::OnSink(float SinkPercent)
{
	for (int i = 0; i < ShipSystems.Num(); i++)
	{
		ShipSystems[i]->OnSink(SinkPercent);
	}
}