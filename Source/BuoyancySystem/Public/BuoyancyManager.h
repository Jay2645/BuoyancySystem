// Copyright 2016 Schemepunk Studios

#pragma once

#include "Components/ActorComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "GameFramework/GameStateBase.h"
#include "BuoyancyManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUOYANCYSYSTEM_API UBuoyancyManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuoyancyManager();
	
	UPROPERTY(Category = "River", EditAnywhere)
	float RiverZLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float MedianWaveOneAmplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	float MedianWaveOneWavelength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	float MedianWaveTwoAmplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	float MedianWaveTwoWavelength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	float Steepness;

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
