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
    /** End of the initialisation of variables for the Keyboard Motion Controller Simulation */

    /** Start of the Grab Functionality Initialisation */
    GrabSphere = CreateDefaultSubobject<USphereComponent>("GrabSphere");
    GrabSphere->AttachTo(this);
    GrabSphere->SetSphereRadius(GrabSphereSize);
    ComponentGrabbed = NULL;

    /** End of the Grab Functionality Initialisation */
}

void UProjectOrionMotionController::BeginPlay()
{
    TArray<USceneComponent*> ArrayOfComponents;
    GetChildrenComponents(true, ArrayOfComponents);
    PhoneAudioComponent = NULL;
    int numberOfChildComponents = ArrayOfComponents.Num();
    for (int i = 0; i < numberOfChildComponents; i++)
    {
        USceneComponent* CurrentComponent = ArrayOfComponents[i];
        UAudioComponent* AudioComponent = Cast<UAudioComponent>(CurrentComponent);
        if (AudioComponent)
        {
            PhoneAudioComponent = AudioComponent;
            IsPhoneHand = true;
            UE_LOG(LogClass, Log, TEXT("Found the audio component in the hand"));
        }
    }

    if (!PhoneAudioComponent)
    {
        IsPhoneHand = false;
        UE_LOG(LogClass, Log, TEXT("DIDN'T FIND the audio component in the hand"));
    }
}

bool UProjectOrionMotionController::GrabComponent()
{
    TArray<AActor*> GrabbableActors;
    UE_LOG(LogClass, Log, TEXT("The size of the grab sphere is %f"), GrabSphere->GetScaledSphereRadius());
    FVector Diff = GrabSphere->GetComponentLocation() - this->GetComponentLocation();
    FString s = Diff.ToString();
    UE_LOG(LogClass, Log, TEXT("The difference in place of grab and controller %s"), *s);

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
                grabbed = TestGrabbable->GrabbedBy(GrabSphere);;
                if (grabbed)
                {
                    ComponentGrabbed = TestGrabbable;
                }
                FString ActorName = GrabbableActors[i]->GetName();
                UE_LOG(LogClass, Log, TEXT("Actor number %i is named %s"), i, *ActorName);
            }
            
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
    UMotionControllerComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


