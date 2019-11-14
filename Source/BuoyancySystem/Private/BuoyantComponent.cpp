// Copyright 2016 Schemepunk Studios

#include "BuoyantComponent.h"
#include "GameFramework/GameState.h"
#include "DrawDebugHelpers.h"

UBuoyant::UBuoyant(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

// Sets default values for this component's properties
UBuoyantComponent::UBuoyantComponent()
{
	// Set this component to be initialized when the game start
	PrimaryComponentTick.bCanEverTick = true;

	WaterTestPoints.Emplace(FVector(-100, 0, 0));
	WaterTestPoints.Emplace(FVector(100, 0, 0));
	WaterTestPoints.Emplace(FVector(0, -100, 0));
	WaterTestPoints.Emplace(FVector(0, 100, 0));
	WaterTestPoints.Emplace(FVector(0, 0, -40));

	PointThickness = 10.0f;
	DisplacementRatio = 1.0f;
	MassMultiplier = 1.0f;
	bShouldFloat = true;

	CurrentSinkTime = 0.0f;
	SinkTime = 5.0f;
}

void UBuoyantComponent::BeginPlay()
{
	Super::BeginPlay();
	if (RiverManager == NULL)
	{
		RiverManager = UBuoyancyManager::GetManager(GetWorld());
	}
	if (PhysicsComponent == NULL)
	{
		AActor* owner = GetOwner();
		if (owner == NULL)
		{
			return;
		}
		TArray<UActorComponent*> allComponents = owner->GetComponents().Array();
		for (int i = 0; i < allComponents.Num(); i++)
		{
			if (allComponents[i] == NULL)
			{
				continue;
			}
			if (allComponents[i]->IsA<UPrimitiveComponent>())
			{
				PhysicsComponent = (UPrimitiveComponent*)allComponents[i];
				break;
			}
		}
	}
}

// Called every frame
void UBuoyantComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (RiverManager == NULL || PhysicsComponent == NULL)
	{
		return;
	}
	ProcessWaveHeight();
	if (!bShouldFloat)
	{
		if(!PhysicsComponent->IsGravityEnabled())
		{
			PhysicsComponent->SetEnableGravity(true);
		}
		if(CurrentSinkTime < SinkTime)
		{
			CurrentSinkTime += DeltaTime;
			if (CurrentSinkTime > SinkTime)
			{
				CurrentSinkTime = SinkTime;
			}
			OnSink();
		}
	}
}

void UBuoyantComponent::SetPhysicsMesh(UPrimitiveComponent* PhysicsMeshComponent)
{
	PhysicsComponent = PhysicsMeshComponent;
}

void UBuoyantComponent::ProcessWaveHeight()
{
	if (!bShouldFloat && !bDrawDebugSpheres)
	{
		return;
	}
	FTransform actorTransform = GetOwner()->GetTransform();
	// Check to see if we should sink
	for (int i = 0; i < WaterSinkPoints.Num(); i++)
	{
		FVector currentPoint = WaterSinkPoints[i];
		FWavePointData waveData = ProcessWaveHeightAtPoint(currentPoint, actorTransform);
		if (waveData.bIsUnder)
		{
			bShouldFloat = false;
			if(!bDrawDebugSpheres)
			{
				break;
			}
		}

		if (bDrawDebugSpheres)
		{
			FColor sphereColor = FColor(0,0,255);
			if (waveData.bIsUnder)
			{
				sphereColor = FColor(255,0,0);
			}
			FVector transformedPoint = actorTransform.TransformPosition(currentPoint);
			DrawDebugSphere(GetWorld(), transformedPoint, PointThickness, 32, sphereColor);
		}
	}

	if (!bShouldFloat)
	{
		if(CurrentSinkTime == 0.0f)
		{
			OnStartSink();
		}
		else if (CurrentSinkTime >= SinkTime)
		{
			return;
		}
	}

	float sinkingMassOffset = 1.0f - (CurrentSinkTime / SinkTime);

	// Buoyancy effects
	float massOfPoint = (PhysicsComponent->GetMass() / WaterTestPoints.Num());

	for (int32 i = 0; i < WaterTestPoints.Num(); i++)
	{
		FVector currentPoint = WaterTestPoints[i];
		FVector transformedPoint = actorTransform.TransformPosition(currentPoint);
		FWavePointData waveData = ProcessWaveHeightAtPoint(currentPoint, actorTransform);

		FVector gravityAmount;
		if (waveData.bIsUnder)
		{
			FVector gravityVector = FVector(0.0f, 0.0f, 980.0f) * DisplacementRatio * sinkingMassOffset;
			gravityAmount = gravityVector * massOfPoint * waveData.Amount;
		}
		else
		{
			FVector gravityVector = FVector(0.0f, 0.0f, -980.0f);
			gravityAmount = gravityVector * waveData.Amount * massOfPoint * MassMultiplier;
		}
		PhysicsComponent->AddForceAtLocation(gravityAmount, transformedPoint);

		if (bDrawDebugSpheres)
		{
			DrawDebugSphere(GetWorld(), transformedPoint, PointThickness, 32, FColor(0, 255, 0));
			DrawDebugLine(GetWorld(), transformedPoint, transformedPoint + gravityAmount, FColor(0, 255, 0));
		}
	}
}

FWavePointData UBuoyantComponent::ProcessWaveHeightAtPoint(FVector pointLocation, const FTransform& actorTransform)
{
	FWavePointData waveData;
	FVector transformedPoint = actorTransform.TransformPosition(pointLocation);
	FVector waveHeight = RiverManager->GetWaveHeightValue(transformedPoint, GetWorld()->GetTimeSeconds());
	waveData.WaveHeight = waveHeight.Z;

	float heightDifference = waveHeight.Z - transformedPoint.Z;
	waveData.Delta = heightDifference;
	waveData.bIsUnder = heightDifference > 0.0f;
	float displacementAmount = FMath::Abs(FMath::Clamp(FMath::Pow(heightDifference / PointThickness, 3.0f), -1.0f, 1.0f));
	waveData.Amount = displacementAmount;

	FVector waveHeightSum = transformedPoint + waveHeight;
	waveData.WaveIntersectPoint = FVector(waveHeightSum.X, waveHeightSum.Y, transformedPoint.Z);

	return waveData;
}

void UBuoyantComponent::OnStartSink()
{
	UE_LOG(LogTemp, Error, TEXT("Broadcasting OnStartSink!"));
	OnStartSinkDelegate.Broadcast();
}

void UBuoyantComponent::OnSink()
{
	OnSinkDelegate.Broadcast(CurrentSinkTime / SinkTime);
}