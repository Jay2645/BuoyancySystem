// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WheeledVehicle.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BuoyantComponent.h"
#include "Systems/ShipSystem.h"
#include "Systems/BowShipVehicleWheel.h"
#include "Systems/SternShipVehicleWheel.h"
#include "BoatVehicle.generated.h"

/**
 * 
 */
UCLASS()
class BUOYANCYSYSTEM_API ABoatVehicle : public AWheeledVehicle, public IBuoyant
{
	GENERATED_BODY()
public:
	ABoatVehicle();

	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* CameraArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stencil")
	UStaticMeshComponent* WaterStencil;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buoyancy")
	UBuoyantComponent* BuoyantComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	TSubclassOf<UBowShipVehicleWheel> BowShipWheelClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
	TSubclassOf<USternShipVehicleWheel> SternShipWheelClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ship")
	TArray<UShipSystem*> ShipSystems;
protected:
	UBuoyantComponent* GetBuoyancyComponent_Implementation();
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Buoyancy")
	UBuoyantComponent* GetBuoyancyComponent();

public:
	UFUNCTION(BlueprintCallable, Category="Ship")
	void RegisterShipSystem(UShipSystem* ShipSystem);

	UFUNCTION(BlueprintCallable, Category="Ship|Control")
	void SetThrottle(float ThrottleAmount);
	UFUNCTION(BlueprintCallable, Category = "Ship|Control")
	void SetSteering(float SteeringAmount);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sinking")
	bool bIsSinking;

	UFUNCTION()
	void OnStartSink();
	UFUNCTION()
	void OnSink(float SinkPercent);
};
