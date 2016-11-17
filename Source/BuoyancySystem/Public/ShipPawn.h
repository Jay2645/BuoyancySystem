// Copyright 2016 Schemepunk Studios

#pragma once

#include "GameFramework/Pawn.h"
#include "BuoyantComponent.h"
#include "ShipPawn.generated.h"

UCLASS()
class BUOYANCYSYSTEM_API AShipPawn : public APawn, public IBuoyant
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShipPawn();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
		UBuoyantComponent* BuoyancyComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
		//UPhysicsComponent* PhysicsComponent;

	UPROPERTY(Category = "Mesh", EditAnywhere)
		UStaticMeshComponent* PawnMesh;

	UPROPERTY(Category = "Boat", EditAnywhere)
		FVector PropellerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat")
		FVector RudderLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boat")
		float MaxRudderAngle;

	UPROPERTY(Category = "Boat Controls", EditAnywhere)
		float MaxSpeed;

	UPROPERTY(Category = "Boat Controls", EditAnywhere)
		float AccelerationPerFrame;
	UPROPERTY(Category = "Boat Controls", EditAnywhere)
		float MaxAcceleration;

	UPROPERTY(Category = "Boat Controls", EditAnywhere)
		float TurnSpeed;

	UPROPERTY(Category = "Camera", EditAnywhere)
		USpringArmComponent* SpringArm;

	UPROPERTY(Category = "Camera", EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(Category = "Movement", VisibleAnywhere)
		FVector2D MovementInput;
	UPROPERTY(Category = "Movement", VisibleAnywhere)
		FVector2D CameraInput;

protected:
	virtual UBuoyantComponent* GetBuoyancyComponent_Implementation();
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Buoyancy")
	UBuoyantComponent* GetBuoyancyComponent();
	//virtual UBuoyancyComponent* GetBuoyancyComponent() override;
	//virtual UPhysicsComponent* GetPhysicsComponent() override;

private:

	void ChangeThrottle(float axisValue);
	void AdjustTurn(float axisValue);

	void PitchCamera(float axisValue);
	void YawCamera(float axisValue);

	void Accelerate();

	float CurrentAcceleration;
};
