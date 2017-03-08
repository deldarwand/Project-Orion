// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionPSMotionController.h"
#include "OWL/PhaseSpaceThread.h"
#include "ProjectOrionCharacter.h"

UProjectOrionPSMotionController::UProjectOrionPSMotionController()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    PlayerIndex = 0;
    bDisableLowLatencyUpdate = false;
	PhaseSpaceThreadInstance = PhaseSpaceThread::StartPhaseSpace();
	PhaseSpaceOffset = FVector::ZeroVector;
	DidSetOffset = false;
	
}

void UProjectOrionPSMotionController::BeginPlay()
{
	FVector Position;
	FRotator Orientation;
	PollControllerState(Position, Orientation);
	PhaseSpaceOffset = Position;
	InitialPosition =  GetRelativeTransform().GetLocation();
	InitialFollowPosition = FollowComponent->GetActorLocation();
}

void UProjectOrionPSMotionController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	FVector Position;
	FRotator Orientation;

	bool bTracked = PollControllerState(Position, Orientation);
	
	FVector OffsetFromOrigin = Position - PhaseSpaceOffset;
	FVector CurrentFollow = FollowComponent->GetActorLocation();
	Position = InitialPosition + OffsetFromOrigin + (CurrentFollow - InitialFollowPosition);
    if (bTracked)
    {
		if (Hand == EControllerHand::Left)
		{
			UE_LOG(LogTemp, Warning, TEXT("Got LEFT location %s"), *Position.ToString());
		}
		else if (Hand == EControllerHand::Right)
		{
			UE_LOG(LogTemp, Warning, TEXT("Got RIGHT location %s"), *Position.ToString());
		}
		//SetWorldLocation(Position);
		SetRelativeLocationAndRotation(Position, Orientation);
    }
}

bool UProjectOrionPSMotionController::PollControllerState(FVector& Position, FRotator& Orientation)
{
	// Makes sure that it is safe to access the marker positions and then gets them and returns them.
	PhaseSpaceThreadInstance->CanAccessMarkers->Lock();
	if (Hand == EControllerHand::Right)
	{
		Position = ConvertFromPSToUE(PhaseSpaceThreadInstance->RightLegPosition);
	}
	else if (Hand == EControllerHand::Left)
	{
		Position = ConvertFromPSToUE(PhaseSpaceThreadInstance->LeftLegPosition);
	}
	PhaseSpaceThreadInstance->CanAccessMarkers->Unlock();
	Orientation = FRotator(0.0f, 0.0f, 0.0f);
    return true;
}

FVector UProjectOrionPSMotionController::ConvertFromPSToUE(FVector PSPosition)
{
	FVector UEPosition;
	UEPosition.X = PSPosition.X;
	UEPosition.Y = PSPosition.Z;
	UEPosition.Z = PSPosition.Y;
	UEPosition /= 10.0f;
	return UEPosition;
}