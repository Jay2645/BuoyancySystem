// Copyright 2016 Schemepunk Studios

#include "BuoyancySystemPrivatePCH.h"
#include "BoatVehicle.h"
#include "ShipSystem.h"

void UShipSystem::BeginPlay()
{
	ABoatVehicle* boat = (ABoatVehicle*)GetOwner();
	boat->RegisterShipSystem(this);
}

bool UShipSystem::OnStartSink_Implementation()
{
	bIsSinking = true;
	return true;
}

bool UShipSystem::OnSink_Implementation(float PercentSunk)
{
	if (!bIsSinking)
	{
		OnStartSink();
	}
	return true;
}

bool UShipSystem::UpdateEngineRPM_Implementation(float CurrentRPM)
{
	return true;
}

bool UShipSystem::UpdateSteering_Implementation(float CurrentSteeringAmount)
{
	return true;
}