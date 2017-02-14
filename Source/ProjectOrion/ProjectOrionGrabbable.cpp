// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionGrabbable.h"


// Sets default values
AProjectOrionGrabbable::AProjectOrionGrabbable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GrabMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrabMesh"));
    RootComponent = GrabMesh;
    Attached = false;
    ComponentToFollow = NULL;

}

// Called when the game starts or when spawned
void AProjectOrionGrabbable::BeginPlay()
{
	Super::BeginPlay();
    AxisLocked = FVector(0.0, 1.0, 0.0);
    OriginalPosition = GetActorLocation();
    MovementLocked = FVector(0.0, 10.0, 0.0);
}

// Called every frame
void AProjectOrionGrabbable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    float XMovement, YMovement, ZMovement;
    FVector CurrentLocation = GetActorLocation();
    if (ShouldUseLockedAxis && ComponentToFollow)
    {
        //Move after the ComponentToFollow taking into account the axisLocked variables.

        FVector FollowLocation = ComponentToFollow->GetComponentLocation();
        FVector LocationDifference = FollowLocation - CurrentLocation;
        XMovement = LocationDifference.X * AxisLocked.X;
        YMovement = LocationDifference.Y * AxisLocked.Y;
        ZMovement = LocationDifference.Z * AxisLocked.Z;



    }
    if (ShouldUseMovementLocked && ComponentToFollow)
    {
        
        FVector FollowLocation = ComponentToFollow->GetComponentLocation();
        FVector LocationDifference = CurrentLocation - OriginalPosition;
        FVector TestLocation = CurrentLocation + FVector(XMovement, YMovement, ZMovement);
        FVector OriginalToTest = TestLocation - OriginalPosition;
        if (abs(OriginalToTest.X) > MovementLocked.X)
        {
            if (OriginalToTest.X < 0.0f)
            {
                float CurrentX = CurrentLocation.X;
                float MaxX = OriginalPosition.X - MovementLocked.X;
                float MovementToTarget = MaxX - CurrentX;
                XMovement = MovementToTarget;
            }
            else
            {

            }
        }
        if (abs(OriginalToTest.Y) > MovementLocked.Y)
        {
            if (OriginalToTest.Y < 0.0f)
            {
                float CurrentY = CurrentLocation.Y;
                float MaxY = OriginalPosition.Y - MovementLocked.Y;
                float MovementToTarget = MaxY - CurrentY;
                YMovement = MovementToTarget;
            }
            else
            {
                float CurrentY = CurrentLocation.Y;
                float MaxY = OriginalPosition.Y + MovementLocked.Y;
                float MovementToTarget = MaxY - CurrentY;
                YMovement = MovementToTarget;
            }
        }
        if (abs(OriginalToTest.Y) > MovementLocked.Y)
        {
            if (OriginalToTest.Z < 0.0f)
            {

            }
            else
            {

            }
        }


    }
    FVector NewLocation = CurrentLocation + FVector(XMovement, YMovement, ZMovement);
    UE_LOG(LogClass, Log, TEXT("New X, New Y, New Z"), i, *ActorName);
    SetActorLocation(NewLocation);
}

bool AProjectOrionGrabbable::GrabbedBy(class USceneComponent* componentToAttachTo)
{
    if (ShouldUseLockedAxis)
    {
        Attached = true;
        ComponentToFollow = componentToAttachTo;
        return true;
    }

    bool Success = true;
    GrabMesh->SetSimulatePhysics(false);
    if (GrabMesh->AttachToComponent(componentToAttachTo, FAttachmentTransformRules::KeepWorldTransform) && !Attached)
    {
        UE_LOG(LogClass, Log, TEXT("Success"));
    }
    else
    {
        UE_LOG(LogClass, Log, TEXT("Fail"));
        Success = false;
    }
    if (Attached)
    {
        Success = false;
    }

    return Success;
}

void AProjectOrionGrabbable::ReleasedBy(class USceneComponent* componentToAttachTo)
{
    if (ShouldUseLockedAxis)
    {
        Attached = false;
        ComponentToFollow = NULL;
        return;
    }
    Attached = false;
    GrabMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    GrabMesh->SetSimulatePhysics(true);
}