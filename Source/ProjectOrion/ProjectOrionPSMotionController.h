// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MotionControllerComponent.h"
#include "ProjectOrionPSMotionController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTORION_API UProjectOrionPSMotionController : public UMotionControllerComponent
{
    GENERATED_BODY()

    UProjectOrionPSMotionController();
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:

    enum EControllerLeg Leg;

private:
    bool PollControllerState(FVector& Position, FRotator& Orientation);
	
	
};

enum EControllerLeg
{
    Left,
    Right
};