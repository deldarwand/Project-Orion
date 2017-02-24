// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionPSMotionController.h"
#include "OWL/PhaseSpaceTracker.hpp"


UProjectOrionPSMotionController::UProjectOrionPSMotionController()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    PlayerIndex = 0;
    bDisableLowLatencyUpdate = false;
	LegTracker = PhaseSpaceTracker::instance();
}

void UProjectOrionPSMotionController::BeginPlay()
{

}

void UProjectOrionPSMotionController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    FVector Position;
    FRotator Orientation;
    bool bTracked = PollControllerState(Position, Orientation);
    if (bTracked)
    {
        SetRelativeLocationAndRotation(Position, Orientation);
    }
}

bool UProjectOrionPSMotionController::PollControllerState(FVector& Position, FRotator& Orientation)
{
    // Use the PhaseSpace library to get position and rotation for the controller.
    Position = GetComponentLocation();
    Orientation = GetComponentRotation();

	std::vector<float> footLocation;
	if (Hand == EControllerHand::Left)
	{
		footLocation = LegTracker->GetLeftFoot();
	}
	else if (Hand == EControllerHand::Right)
	{
		footLocation = LegTracker->GetRightFoot();
	}
	FVector footLocationV = FVector(footLocation[0], footLocation[1], footLocation[2]);
	footLocationV = ConvertFromPSToUE(footLocationV);
	
	Position = footLocationV;

    return true;
}

FVector UProjectOrionPSMotionController::ConvertFromPSToUE(FVector PSPosition)
{
	FVector UEPosition;

	for (int i = 0; i < 3; i++)
	{
		UEPosition[i] = (PSPosition[i] / 10.0f);
	}
	return UEPosition;
}