// Copyright 2016 Schemepunk Studios

#pragma once

#include "Components/SceneComponent.h"
#include "ShipSystem.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUOYANCYSYSTEM_API UShipSystem : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShipSystem() {};

	virtual void BeginPlay() override;

protected:
	virtual bool OnStartSink_Implementation();
	virtual bool OnSink_Implementation(float PercentSunk);

	virtual bool UpdateEngineRPM_Implementation(float CurrentRPM);
	virtual bool UpdateSteering_Implementation(float CurrentSteeringAmount);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sinking")
	bool bIsSinking;
	
	UFUNCTION(BlueprintNativeEvent, Category="Sinking")
	bool OnStartSink();
	UFUNCTION(BlueprintNativeEvent, Category = "Sinking")
	bool OnSink(float PercentSunk);

	UFUNCTION(BlueprintNativeEvent, Category = "Control")
	bool UpdateEngineRPM(float CurrentRPM);
	UFUNCTION(BlueprintNativeEvent, Category = "Control")
	bool UpdateSteering(float CurrentSteeringAmount);
};
