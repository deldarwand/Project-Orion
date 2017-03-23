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
	DidGetVivePosition = false;
	NumberOfSkippedFrames = 0;
}

void UProjectOrionPSMotionController::BeginPlay()
{
	if (IsLeg)
	{
		Hand = EControllerHand::Right;
	}
	if (GetAttachParent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Got attach parent. %s"), *(GetAttachParent()->GetName()));
		InitialFoot = GetAttachParent()->GetComponentLocation() - GetComponentLocation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DIDN'T Get attach parent."));
	}
	FVector Position;
	FRotator Orientation;
	PollControllerState(Position, Orientation);
	PhaseSpaceOffset = Position;
	InitialPosition = GetComponentLocation();
	InitialPosition.Z = 0.0f;
	TArray<USceneComponent*> ArrayOfParents;
	GetParentComponents(ArrayOfParents);
	for (int i = 0; i < ArrayOfParents.Num(); i++)
	{
		UCameraComponent* Component = Cast<UCameraComponent>(ArrayOfParents[i]);
		if (Component)
		{
			FollowComponent = Component;
		}
	}
    if (FollowComponent)
    {
		InitialFollowPosition = FollowComponent->GetComponentLocation();
    }
}

void UProjectOrionPSMotionController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (NumberOfSkippedFrames < NUMBER_OF_FRAMES_TO_SKIP)
	{
		NumberOfSkippedFrames++;
		return;
	}
	if (!DidGetVivePosition)
	{
		InitialVivePosition = GetComponentLocation();//FollowComponent->GetComponentLocation();
		DidGetVivePosition = true;
	}
	FVector Position;
	FRotator Orientation;
	FVector RelativePosition;
	bool bTracked = PollControllerState(Position, Orientation);
    if (!PhaseSpaceThreadInstance->FoundPhaseSpace)
    {
        return;
    }
    if (FollowComponent)
    {
		FRotator PSToVIVE = FRotator(0.0f, -12.7f, 0.0f);
        FVector OffsetFromOrigin = Position - PhaseSpaceOffset;
		OffsetFromOrigin = PSToVIVE.RotateVector(OffsetFromOrigin);
        FVector CurrentFollow = FollowComponent->GetComponentLocation();
		UE_LOG(LogTemp, Warning, TEXT("RAW PS Position is %s."), *Position.ToString());
		UE_LOG(LogTemp, Warning, TEXT("VIVE Position is %s."), *(FollowComponent->GetComponentLocation().ToString()));
		UE_LOG(LogTemp, Warning, TEXT("Initial Position is %s."), *InitialPosition.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Initial Player Position is %s."), *InitialFollowPosition.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Player Position Diff is %s."), *(FollowComponent->GetComponentLocation() - InitialFollowPosition).ToString());
		UE_LOG(LogTemp, Warning, TEXT("Initial VIVE Position is %s."), *InitialVivePosition.ToString());
		Position = OffsetFromOrigin + InitialVivePosition;//InitialPosition + OffsetFromOrigin + (InitialVivePosition - InitialFollowPosition);
		RelativePosition = OffsetFromOrigin + InitialFoot;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot find a player to follow."));
    }

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
		SetWorldLocation(Position);
		//SetRelativeLocationAndRotation(RelativePosition, Orientation);
    }
}

bool UProjectOrionPSMotionController::PollControllerState(FVector& Position, FRotator& Orientation)
{
	// Makes sure that it is safe to access the marker positions and then gets them and returns them.
	PhaseSpaceThreadInstance->CanAccessMarkers->Lock();
	if (Hand == EControllerHand::Right)
	{
		Position = ConvertFromPSToUE(PhaseSpaceThreadInstance->RightLegPosition);
		Orientation = PhaseSpaceThreadInstance->RightLegRotation;
	}
	else if (Hand == EControllerHand::Left)
	{
		Position = ConvertFromPSToUE(PhaseSpaceThreadInstance->LeftLegPosition);
		Orientation = PhaseSpaceThreadInstance->LeftLegRotation;
	}
	PhaseSpaceThreadInstance->CanAccessMarkers->Unlock();
    return true;
}

FVector UProjectOrionPSMotionController::ConvertFromPSToUE(FVector PSPosition)
{
	FVector UEPosition;
	UEPosition.Y = -PSPosition.Z;
	UEPosition.X = -PSPosition.X;
	UEPosition.Z = PSPosition.Y;
	UEPosition /= 10.0f;
	return UEPosition;
}