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

}

// Called when the game starts or when spawned
void AProjectOrionGrabbable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectOrionGrabbable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

bool AProjectOrionGrabbable::GrabbedBy(class USceneComponent* componentToAttachTo)
{
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
    Attached = false;
    GrabMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    GrabMesh->SetSimulatePhysics(true);
}