// Fill out your copyright notice in the Description page of Project Settings.

#include "BuoyancySystemPrivatePCH.h"
#include "EngineSystem.h"

UEngineSystem::UEngineSystem()
{
	EngineSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Engine Sound"));
	EngineSound->AttachToComponent(this,FAttachmentTransformRules(EAttachmentRule::KeepWorld,false));
	EngineSound->SetRelativeLocation(FVector::ZeroVector);
}

bool UEngineSystem::OnSink_Implementation(float PercentSunk)
{
	EngineSound->SetPitchMultiplier(EngineSound->PitchMultiplier * (1.0f - (PercentSunk * 2.0f)));
	EngineSound->SetVolumeMultiplier(1.0f - PercentSunk);
	return true;
}

bool UEngineSystem::UpdateEngineRPM_Implementation(float CurrentRPM)
{
	if(!bIsSinking)
	{
		EngineSound->SetPitchMultiplier(CurrentRPM);
	}
	return true;
}