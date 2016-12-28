// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionMotionController.h"
#include "ProjectOrionGrabbable.h"

UProjectOrionMotionController::UProjectOrionMotionController()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    PlayerIndex = 0;

    bDisableLowLatencyUpdate = false;
    
    /** Start of the initialisation of variables for the Keyboard Motion Controller Simulation */
    ShouldMoveLeft = ShouldMoveRight = ShouldMoveForward = ShouldMoveBack = ShouldMoveUp = ShouldMoveDown = false;
    ShouldRotateAroundX = ShouldRotateAroundY = ShouldRotateAroundZ = false;
    TranslationSpeedVector = FVector(50.0f);

    ControllerPosition = FVector(0.0f);
    ControllerOrientation = FRotator(0.0f);
    RotationSpeedRotator = FRotator(50.0f, 50.0f, 50.0f);
    /** End of the initialisation of variables for the Keyboard Motion Controller Simulation */

    /** Start of the Grab Functionality Initialisation */
    GrabSphere = CreateDefaultSubobject<USphereComponent>("GrabSphere");
    GrabSphere->AttachTo(this);
    GrabSphere->SetSphereRadius(GrabSphereSize);
    ComponentGrabbed = NULL;
    /** End of the Grab Functionality Initialisation */
}

void UProjectOrionMotionController::ToggleBoolean(bool* booleanToToggle)
{
    *booleanToToggle = !(*booleanToToggle);
}

void UProjectOrionMotionController::ToggleMoveLeft()
{
	ShouldMoveLeft = !ShouldMoveLeft;
}

void UProjectOrionMotionController::ToggleMoveRight()
{
	ShouldMoveRight = !ShouldMoveRight;
}

void UProjectOrionMotionController::ToggleMoveForward()
{
	ShouldMoveForward = !ShouldMoveForward;
}

void UProjectOrionMotionController::ToggleMoveBack()
{
	ShouldMoveBack = !ShouldMoveBack;
}

void UProjectOrionMotionController::ToggleMoveUp()
{
	ShouldMoveUp = !ShouldMoveUp;
}

void UProjectOrionMotionController::ToggleMoveDown()
{
	ShouldMoveDown = !ShouldMoveDown;
}



void UProjectOrionMotionController::ToggleXRotation()
{
	ShouldRotateAroundX = !ShouldRotateAroundX;
}

void UProjectOrionMotionController::ToggleYRotation()
{
	ShouldRotateAroundY = !ShouldRotateAroundY;	
}

void UProjectOrionMotionController::ToggleZRotation()
{
	ShouldRotateAroundZ = !ShouldRotateAroundZ;
}

void UProjectOrionMotionController::TranslateController(FVector TranslationVector)
{
    ControllerPosition += TranslationVector;
}

void UProjectOrionMotionController::RotateController(FRotator RotationRotator)
{
	ControllerOrientation += RotationRotator;
}

bool UProjectOrionMotionController::PollControllerState(FVector& Position, FRotator& Orientation)
{
	Position = ControllerPosition;
	Orientation = ControllerOrientation;
	return true;
}

bool UProjectOrionMotionController::GrabComponent()
{
    TArray<AActor*> GrabbableActors;
    if (GrabSphere)
    {
        GrabSphere->GetOverlappingActors(GrabbableActors);
        UE_LOG(LogClass, Log, TEXT("There are %i actors that can be grabbed."), GrabbableActors.Num());
        UE_LOG(LogClass, Log, TEXT("My Name: %s."), *(GetName()));
        bool grabbed = false;
        for (int i = 0; i < GrabbableActors.Num(); i++)
        {
            AProjectOrionGrabbable* const TestGrabbable = Cast<AProjectOrionGrabbable>(GrabbableActors[i]);
            if (TestGrabbable && !grabbed)
            {
                grabbed = true;
                TestGrabbable->GrabbedBy(GrabSphere);
                ComponentGrabbed = TestGrabbable;
            }
            FString ActorName = GrabbableActors[i]->GetName();
            UE_LOG(LogClass, Log, TEXT("Actor number %i is named %s"), i, *ActorName);
        }
    }
    else
    {
        UE_LOG(LogClass, Warning, TEXT("Grab sphere not initialised."));
    }
    return true;
}

bool UProjectOrionMotionController::ReleaseComponent()
{
    if (ComponentGrabbed)
    {
        ComponentGrabbed->ReleasedBy(GrabSphere);
        ComponentGrabbed = NULL;
    }
    return true;
}

void UProjectOrionMotionController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (!ShouldBeSimulated)
	{
		UMotionControllerComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
		return;
	}
	FVector Position = FVector(0.0f);
	FRotator Orientation;
	FVector DeltaMovement = TranslationSpeedVector * DeltaTime;
	FRotator DeltaRotation = RotationSpeedRotator* DeltaTime;
	FVector TranslationVector = FVector(0.0f);

	if (ShouldMoveLeft)
	{
		TranslationVector.Y -= DeltaMovement.Y;
	}
	if (ShouldMoveRight)
	{
		TranslationVector.Y += DeltaMovement.Y;
	}
	if (ShouldMoveForward)
	{
		TranslationVector.X -= DeltaMovement.X;
	}
	if (ShouldMoveBack)
	{
		TranslationVector.X += DeltaMovement.X;
	}
	if (ShouldMoveDown)
	{
		TranslationVector.Z -= DeltaMovement.Z;
	}
	if (ShouldMoveUp)
	{
		TranslationVector.Z += DeltaMovement.Z;
	}

	FRotator RotationRotator = FRotator(0.0f);
	if (ShouldRotateAroundX)
	{
		RotationRotator.Roll += DeltaRotation.Roll;
	}
	if (ShouldRotateAroundY)
	{
		RotationRotator.Pitch += DeltaRotation.Pitch;
	}
	if (ShouldRotateAroundZ)
	{
		RotationRotator.Yaw += DeltaRotation.Yaw;
	}

	RotateController(RotationRotator);
	TranslateController(TranslationVector);

	bool bTracked = PollControllerState(Position, Orientation);
	if (bTracked)
	{
		SetRelativeLocationAndRotation(Position, Orientation);
	}
}


