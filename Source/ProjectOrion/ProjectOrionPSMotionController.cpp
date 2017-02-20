// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionPSMotionController.h"


UProjectOrionPSMotionController::UProjectOrionPSMotionController()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    PlayerIndex = 0;
    bDisableLowLatencyUpdate = false;
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
    return true;
}