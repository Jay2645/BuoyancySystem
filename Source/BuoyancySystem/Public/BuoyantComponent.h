// Copyright 2016 Schemepunk Studios

#pragma once

#include "Components/ActorComponent.h"
#include "BuoyancyManager.h"
#include "BuoyantComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartSinkDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSinkDelegate,float,SinkPercent);

class UBuoyantComponent;

USTRUCT(BlueprintType)
struct FWavePointData
{
	GENERATED_BODY()

		UPROPERTY()
		bool bIsUnder;

	UPROPERTY()
		float Amount;

	UPROPERTY()
		float Delta;

	UPROPERTY()
		FVector WaveIntersectPoint;

	UPROPERTY()
		float WaveHeight;

	//Constructor
	FWavePointData()
	{
		bIsUnder = false;
	}
};

UINTERFACE(BlueprintType)
class BUOYANCYSYSTEM_API UBuoyant : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class BUOYANCYSYSTEM_API IBuoyant
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Buoyancy")
	UBuoyantComponent* GetBuoyancyComponent();
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUOYANCYSYSTEM_API UBuoyantComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuoyantComponent();

	void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetPhysicsMesh(UPrimitiveComponent* PhysicsMeshComponent);

public:
	UPROPERTY(Category = "Water", EditAnywhere, BlueprintReadWrite)
	UBuoyancyManager* RiverManager;

	UPROPERTY(Category = "Water", EditAnywhere, BlueprintReadWrite)
	TArray<FVector> WaterTestPoints;

	UPROPERTY(Category = "Water", EditAnywhere, BlueprintReadWrite)
	TArray<FVector> WaterSinkPoints;

	UPROPERTY(Category = "Water", EditAnywhere, BlueprintReadWrite)
	float PointThickness;

	UPROPERTY(Category = "Water", EditAnywhere, BlueprintReadWrite)
	float DisplacementRatio;

	UPROPERTY(Category = "Water", EditAnywhere, BlueprintReadWrite)
	float MassMultiplier;

	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadWrite)
	bool bDrawDebugSpheres;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
	UPrimitiveComponent* PhysicsComponent;

protected:
	float CurrentSinkTime;
	UFUNCTION()
		void OnStartSink();
	UFUNCTION()
		void OnSink();
public:
	FStartSinkDelegate OnStartSinkDelegate;
	FOnSinkDelegate OnSinkDelegate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sinking")
	bool bShouldFloat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sinking")
	float SinkTime;

private:
	void ProcessWaveHeight();
public:
	FWavePointData ProcessWaveHeightAtPoint(FVector pointLocation, const FTransform& actorTransform);
};