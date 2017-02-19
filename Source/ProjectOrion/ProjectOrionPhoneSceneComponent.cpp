// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionPhoneSceneComponent.h"


// Sets default values for this component's properties
UProjectOrionPhoneSceneComponent::UProjectOrionPhoneSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
    
    TimeToWait = 5.0f;
    WaitedFor = 0.0f;


	// ...
}


// Called when the game starts
void UProjectOrionPhoneSceneComponent::BeginPlay()
{
	Super::BeginPlay();
    CurrentPhoneState = PhoneState::Calling;
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
            UE_LOG(LogClass, Log, TEXT("Found the audio component in the hand"));
        }
    }

    if (!PhoneAudioComponent)
    {
        UE_LOG(LogClass, Log, TEXT("DIDN'T FIND the audio component in the hand"));
    }

    if (PhoneAttenuation && PhoneAudioComponent)
    {
        PhoneAudioComponent->bAllowSpatialization = true;
        PhoneAudioComponent->AttenuationSettings = PhoneAttenuation;
    }
	// ...
	
}

void UProjectOrionPhoneSceneComponent::AnswerPhone()
{
    CurrentPhoneState = PhoneState::Introduction;
}

// Called every frame
void UProjectOrionPhoneSceneComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if(CurrentPhoneState == PhoneState::Calling)
    {
        ShouldProduceHaptic = true;
    }
    else
    {
        ShouldProduceHaptic = false;
    }

    switch ((int)CurrentPhoneState)
    {

    case PhoneState::Introduction:
    {
        if (IntroductionAudio && !PhoneAudioComponent->IsPlaying())
        {
            PhoneAudioComponent->Activate();
            PhoneAudioComponent->SetSound(IntroductionAudio);
            PhoneAudioComponent->Play();
        }
        break;
    }
    default:
        break;
    }
	// ...
}

