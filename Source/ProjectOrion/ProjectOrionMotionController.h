// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MotionControllerComponent.h"
#include "ProjectOrionMotionController.generated.h"

/**
 * This class deals with the simulation of the motion controller using the keyboard.
	It allows for the movement and rotation of the motion controller using the keyboard
	as well as the use of a standard motion controller if ShouldBeSimulated is set to false
	in the blueprint or editor.
 */

UCLASS()
class PROJECTORION_API UProjectOrionMotionController : public UMotionControllerComponent
{
	GENERATED_BODY()
	
public:
	UProjectOrionMotionController();
	void TranslateController(FVector TranslationVector);
	void RotateController(FRotator RotationRotator);

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


    bool GrabComponent();
    bool ReleaseComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Sphere Size", meta = (AllowPrivateAccess = "true"))
    float GrabSphereSize = 50.0f;

    class USphereComponent* GrabSphere;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Simulation", meta = (AllowPrivateAccess = "true"))
    EControllerHand StartHand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone", meta = (AllowPrivateAccess = "true"))
    bool IsPhoneHand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phone Audio")
    class USoundBase* IntroductionAudio;


    friend class AProjectOrionCharacter;

    void BeginPlay();

private:
    UAudioComponent* PhoneAudioComponent;
	
    class AProjectOrionGrabbable* ComponentGrabbed;

	bool PollControllerState(FVector& Position, FRotator& Orientation);
	
};