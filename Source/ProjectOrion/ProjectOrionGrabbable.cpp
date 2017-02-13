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
}

// Called every frame
void AProjectOrionGrabbable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ShouldUseLockedAxis && ComponentToFollow)
    {
        //Move after the ComponentToFollow taking into account the axisLocked variables.
        float XMovement, YMovement, ZMovement;
        FVector CurrentLocation = GetActorLocation();
        FVector FollowLocation = ComponentToFollow->GetComponentLocation();
        FVector LocationDifference = FollowLocation - CurrentLocation;
        XMovement = LocationDifference.X * AxisLocked.X;
        YMovement = LocationDifference.Y * AxisLocked.Y;
        ZMovement = LocationDifference.Z * AxisLocked.Z;

        FVector NewLocation = CurrentLocation + FVector(XMovement, YMovement, ZMovement);
        SetActorLocation(NewLocation);

    }
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