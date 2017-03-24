// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "ProjectOrionCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AProjectOrionCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ActorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tracked Movement", meta = (AllowPrivateAccess = "true"))
	class UProjectOrionMotionController* LeftHandMotionController;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tracked Movement", meta = (AllowPrivateAccess = "true"))
    class UProjectOrionMotionController* RightHandMotionController;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Phone", meta = (AllowPrivateAccess = "true"))
    class UProjectOrionPhoneSceneComponent* PhoneComponent;

public:
	AProjectOrionCharacter();

	virtual void BeginPlay();

    virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult);

    UFUNCTION(BlueprintCallable, Category = "RayCast")
    void RayCastTick(UCameraComponent* Camera, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Tick")
    void RecordingTick(float DeltaTime);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float BaseRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    float BaseRadiusScale;

    FVector CameraRelativeOffset;

    UFUNCTION(BlueprintCallable, Category = Camera)
    FVector CalculateCameraPosition(FRotator CameraRotator, UCameraComponent* Camera);

    void RadioTouched();

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

    /** Called when user wants to grab object with right hand*/
    void GrabRight();

    /** Called when user wants to let go of object with right hand*/
    void ReleaseRight();

    /** Called when user wants to grab object with left hand*/
    void GrabLeft();

    /** Called when user wants to let go of object with left hand*/
    void ReleaseLeft();

    void AnswerPhone();

    void NextLine();
    void PromptUser();
    void SaveData();
    void SaveData(FString NameModifier = "");
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    virtual void Destroyed();

    void GrabbedWallet();

private:

    FString SavePositions();
    FString SaveRotators();

    TMap<FString, float> LookAtMap;
    FDateTime RecordDateTime;
    int NumberOfPrompts;

    FVector* PositionArray;
    FRotator* RotatorArray;

    FDateTime StartTime;

    // Current index to be recorded into.
    int CurrentRecordingIndex;

};

