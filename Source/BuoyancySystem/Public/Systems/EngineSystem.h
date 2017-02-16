// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Systems/ShipSystem.h"
#include "Components/AudioComponent.h"
#include "EngineSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUOYANCYSYSTEM_API UEngineSystem : public UShipSystem
{
	GENERATED_BODY()
public:
	UEngineSystem();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	UAudioComponent* EngineSound;

public:
	virtual bool OnSink_Implementation(float PercentSunk) override;
	virtual bool UpdateEngineRPM_Implementation(float CurrentRPM) override;
};
