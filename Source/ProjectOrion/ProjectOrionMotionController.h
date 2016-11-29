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
	void ToggleMoveRight();
	void ToggleMoveLeft();
	void ToggleMoveForward();
	void ToggleMoveBack();
	void ToggleMoveUp();
	void ToggleMoveDown();

	void ToggleXRotation();
	void ToggleYRotation();
	void ToggleZRotation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Simulation", meta = (AllowPrivateAccess = "true"))
	bool ShouldBeSimulated;

private:
	FVector ControllerPosition, TranslationSpeedVector;
	FRotator ControllerOrientation, RotationSpeedRotator;
	bool ShouldMoveLeft, ShouldMoveRight, ShouldMoveForward, ShouldMoveBack, ShouldMoveUp, ShouldMoveDown;
	bool ShouldRotateAroundX, ShouldRotateAroundY, ShouldRotateAroundZ;

	bool PollControllerState(FVector& Position, FRotator& Orientation);
	
};