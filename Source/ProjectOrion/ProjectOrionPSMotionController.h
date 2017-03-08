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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Controller")
    bool IsLeg;

private:
    bool PollControllerState(FVector& Position, FRotator& Orientation);
	class PhaseSpaceThread* PhaseSpaceThreadInstance;
	FVector ConvertFromPSToUE(FVector PSPosition);
	FVector PhaseSpaceOffset;
	FVector InitialPosition;
	bool DidSetOffset;

};
