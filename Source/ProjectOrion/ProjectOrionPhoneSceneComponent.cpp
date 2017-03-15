// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionPhoneSceneComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UProjectOrionPhoneSceneComponent::UProjectOrionPhoneSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
    
    TimeToWait = 5.0f;
    WaitedFor = 0.0f;
    ShouldPrompt = IsPrompting = false;
    ShouldPlay = false;
    IsInCall = false;
    NarrativeOffset = (int)PhoneState::Introduction;
    CurrentPrompt = NULL;
    PlayedLastFrame = false;
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

bool UProjectOrionPhoneSceneComponent::PromptUser()
{
    if (!IsPrompting)
    {
        CurrentPrompt = Prompts[UKismetMathLibrary::RandomIntegerInRange(0, 3)];
        ShouldPrompt = true;
        return true;
    }
    return false;
}

void UProjectOrionPhoneSceneComponent::AnswerPhone()
{
    CurrentPhoneState = PhoneState::Introduction;
    IsInCall = true;
    ShouldPlay = true;
}

// Called every frame
void UProjectOrionPhoneSceneComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if (CurrentPhoneState == PhoneState::Calling)
    {
        ShouldProduceHaptic = true;
    }
    else
    {
        ShouldProduceHaptic = false;
    }

    if (ShouldPrompt && !PhoneAudioComponent->IsPlaying() && CurrentPrompt)
    {
        PhoneAudioComponent->Activate();
        PhoneAudioComponent->SetSound(CurrentPrompt);
        PhoneAudioComponent->Play();
        IsPrompting = true;
        ShouldPrompt = false;
    }

    if (IsPrompting)
    {
        IsPrompting = PhoneAudioComponent->IsPlaying();
    }

    if (ShouldPlay)
    {
        UE_LOG(LogClass, Log, TEXT("In should play"));
        if (!PhoneAudioComponent->IsPlaying() && ((int)CurrentPhoneState - NarrativeOffset) < NarrativePrompts.Num())
        {
            PhoneAudioComponent->Activate();
            PhoneAudioComponent->SetSound(NarrativePrompts[(int)CurrentPhoneState - NarrativeOffset]);
            PhoneAudioComponent->Play();
            ShouldPlay = false;
        }

    }
    bool IsPlayingNow = PhoneAudioComponent->IsPlaying();
    if (PlayedLastFrame && !IsPlayingNow)
    {
        UE_LOG(LogClass, Log, TEXT("Phone finished playing. Can keep going."));
    }
    PlayedLastFrame = IsPlayingNow;
}

void UProjectOrionPhoneSceneComponent::SetState(enum PhoneState NewState)
{
    if (NewState == PhoneState::NextAudio)
    {
        CurrentPhoneState = (enum PhoneState)((int)CurrentPhoneState+1);
    }
    else
    {
        CurrentPhoneState = NewState;
    }
    ShouldPlay = true;
}
