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
    class USoundBase* PromptOne;
    
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
    class USoundBase* IntroductionAudio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
    class USoundBase* RadioThanksAudio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Haptic Feedback")
    bool ShouldProduceHaptic;

    void AnswerPhone();

    void SetState(enum PhoneState NewState);

    void PromptUser();

private:
    enum PhoneState CurrentPhoneState;
    float TimeToWait;
    float WaitedFor;
    bool ShouldPrompt, IsPrompting;
    bool ShouldPlay;
};

enum PhoneState
{
    Calling,
    Introduction,
    Radio,
    ThanksRadio,
    NextAudio
};

		
