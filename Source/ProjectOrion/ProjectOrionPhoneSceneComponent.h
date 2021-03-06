// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "ProjectOrionPhoneSceneComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTORION_API UProjectOrionPhoneSceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectOrionPhoneSceneComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds Settings")
    class UAudioComponent* PhoneAudioComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
    class USoundAttenuation* PhoneAttenuation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prompt Sounds")
    TArray<USoundBase*> Prompts;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
    TArray<USoundBase*> NarrativePrompts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
    class USoundBase* CurrentPrompt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Feedback")
    bool ShouldProduceHaptic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone Settings")
    bool IsInCall;
    void AnswerPhone();



    void SetState(enum PhoneState NewState);

    bool PromptUser();

private:
    enum PhoneState CurrentPhoneState;
    float TimeToWait;
    float WaitedFor;
    bool ShouldPrompt, IsPrompting;
    bool ShouldPlay;
    int NarrativeOffset;
    bool PlayedLastFrame;
};

enum PhoneState
{
    Calling = 0,
    Introduction = 1,
    CabinetDrawer = 2,
    InOtherDrawers = 3,
    Windowsill = 4,
    TVStand = 5,
    End = 6,
    ThanksRadio = 7,
    NextAudio = 8
};

		
