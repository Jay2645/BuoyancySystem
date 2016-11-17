// Copyright 2016 Schemepunk Studios

#pragma once

#include "Components/ActorComponent.h"
#include "BuoyancyManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UBuoyancyManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuoyancyManager();
	
	UPROPERTY(Category = "River", EditAnywhere)
	float RiverZLocation;

	UPROPERTY(Category = "Material", EditAnywhere)
	UMaterialParameterCollection* MaterialCollection;

	UFUNCTION(BlueprintCallable, Category = "River|Waves")
	FVector GetWaveHeightValue(FVector location, float time);

private:

	FVector CalculateGerstnerWaveCluster(float medianWavelength, float medianAmplitude, FVector2D position, FVector2D direction, float steepness, float time);
	FVector CalculateGerstnerWave(float wavelength, float amplitude, FVector2D position, FVector2D direction, float angle, float steepness, float time, float phase);

public:
	static UBuoyancyManager* GetManager(UWorld* World);
private:
	static UBuoyancyManager* CreateBuoyancyManager(AGameStateBase* gameState);
};
