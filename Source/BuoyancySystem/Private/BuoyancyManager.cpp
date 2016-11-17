// Copyright 2016 Schemepunk Studios

#include "BuoyancySystemPrivatePCH.h"
#include "Runtime/Engine/Classes/Kismet/KismetMaterialLibrary.h"
#include "BuoyancyManager.h"


// Sets default values for this component's properties
UBuoyancyManager::UBuoyancyManager()
{
	PrimaryComponentTick.bCanEverTick = false;

	RiverZLocation = 0.0f;

	MedianWaveOneAmplitude = 85.0f;
	MedianWaveOneWavelength = 2500.0f;
	MedianWaveTwoAmplitude = 25.0f;
	MedianWaveTwoWavelength = 500.0f;
	Steepness = 0.5f;

	MaterialCollection = NULL;
}

FVector UBuoyancyManager::GetWaveHeightValue(FVector location, float time)
{
	if (MaterialCollection != NULL)
	{
		MedianWaveOneAmplitude = UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, TEXT("Wave 1 Height"));
		MedianWaveOneWavelength = UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, TEXT("Wave 1 Scale"));
		MedianWaveTwoAmplitude = UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, TEXT("Wave 2 Height"));
		MedianWaveTwoWavelength = UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, TEXT("Wave 2 Scale"));
		Steepness = UKismetMaterialLibrary::GetScalarParameterValue(GetWorld(), MaterialCollection, TEXT("Steepness"));
	}
	FVector sum = FVector(0, 0, 0);

	sum += CalculateGerstnerWaveCluster(MedianWaveOneWavelength, MedianWaveOneAmplitude, FVector2D(location.X, location.Y), FVector2D(0, 1), Steepness, time);
	sum += CalculateGerstnerWaveCluster(MedianWaveTwoWavelength, MedianWaveTwoAmplitude, FVector2D(location.X, location.Y), FVector2D(0, 1), Steepness, time);

	return sum / 2;
}

FVector UBuoyancyManager::CalculateGerstnerWaveCluster(float medianWavelength, float medianAmplitude, FVector2D position, FVector2D direction, float steepness, float time)
{
	FVector sum = FVector(0, 0, 0);

	sum += CalculateGerstnerWave(medianWavelength * 1.0f, medianAmplitude * 1.0f, position, direction, 0.0f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 0.5f, medianAmplitude * 0.5f, position, direction, -0.1f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 2.0f, medianAmplitude * 2.0f, position, direction, 0.1f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 1.25f, medianAmplitude * 1.25f, position, direction, 0.05f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 1.25f, medianAmplitude * 1.25f, position, direction, 0.05f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 0.75f, medianAmplitude * 0.75f, position, direction, 0.075f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 1.5f, medianAmplitude * 1.5f, position, direction, -0.125f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 0.825f, medianAmplitude * 0.825f, position, direction, 0.63f, steepness, time, 0.0f);
	sum += CalculateGerstnerWave(medianWavelength * 0.65f, medianAmplitude * 0.65f, position, direction, -0.11f, steepness, time, 0.0f);

	return sum / 8;
}

FVector UBuoyancyManager::CalculateGerstnerWave(float wavelength, float amplitude, FVector2D position, FVector2D direction, float angle, float steepness, float time, float phase)
{
	FVector dir = FVector(direction.X, direction.Y, 0.0f);
	dir = dir.RotateAngleAxis(angle * 360.0f, FVector(0, 0, 1));
	FVector2D rotatedDirection = FVector2D(dir.X, dir.Y);

	float lambda = (2 * PI) / wavelength;
	float wavePhase = lambda * FVector2D::DotProduct(rotatedDirection, position) + (time + phase);
	float sinPhase = FMath::Sin(wavePhase);
	float cosPhase = FMath::Cos(wavePhase);

	float qa = steepness * amplitude;

	return FVector(qa * rotatedDirection.X * cosPhase, qa * rotatedDirection.Y * cosPhase, (amplitude * sinPhase) + RiverZLocation);
}

UBuoyancyManager* UBuoyancyManager::GetManager(UWorld* World)
{
	AGameStateBase* gameState = World->GetGameState();
	UBuoyancyManager* manager = (UBuoyancyManager*)gameState->GetComponentByClass(UBuoyancyManager::StaticClass());
	if (manager == NULL)
	{
		manager = CreateBuoyancyManager(gameState);
	}
	return manager;
}

UBuoyancyManager* UBuoyancyManager::CreateBuoyancyManager(AGameStateBase* gameState)
{
	return NewObject<UBuoyancyManager>(gameState,UBuoyancyManager::StaticClass());
}
