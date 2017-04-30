// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectOrion.h"
#include "ProjectOrionCharacter.h"
#include "ProjectOrionProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Door.h"
#include "ProjectOrionMotionController.h"
#include "ProjectOrionPhoneSceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AProjectOrionCharacter

AProjectOrionCharacter::AProjectOrionCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(false);
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = true;
	Mesh1P->CastShadow = true;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);



	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	ActorMesh->SetupAttachment(FirstPersonCameraComponent);
	ActorMesh->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);

    NumberOfPrompts = 0;
    CurrentRecordingIndex = 0;
    LookAtMap.Empty();

    PositionArray = new FVector[54000];
    RotatorArray = new FRotator[54000];
	TimeArray = new float[54000];
}

void AProjectOrionCharacter::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
    //UE_LOG(LogTemp, Warning, TEXT("Function to calculate camera within Project Orion Character"));
    AActor::CalcCamera(DeltaTime, OutResult);
    //OutResult.Location += FVector(0.0f, 0.0f, 50.0f);
}

void AProjectOrionCharacter::Destroyed()
{
    delete[] PositionArray;
    delete[] RotatorArray;
}

void AProjectOrionCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
    RecordDateTime = FDateTime::Now();
    StartTime = RecordDateTime;
    CameraRelativeOffset = FVector(0.0f, 0.0f, 180.0f);//FirstPersonCameraComponent->RelativeLocation;
    TArray<UActorComponent*> ArrayOfMotionControllers = this->GetComponentsByClass(UProjectOrionMotionController::StaticClass());
    
    for (int i = 0; i < ArrayOfMotionControllers.Num(); i++)
    {
        UProjectOrionMotionController* const MotionController = Cast<UProjectOrionMotionController>(ArrayOfMotionControllers[i]);
        if (i == 0)
        {
            MotionController->Hand = EControllerHand::Left;
            LeftHandMotionController = MotionController;
        }
        if (i == 1)
        {
            MotionController->Hand = EControllerHand::Right;
            RightHandMotionController = MotionController;
        }
    }
    
    TArray<UActorComponent*> ArrayOfPhoneComponents = this->GetComponentsByClass(UProjectOrionPhoneSceneComponent::StaticClass());
    for (int i = 0; i < ArrayOfPhoneComponents.Num(); i++)
    {
        UProjectOrionPhoneSceneComponent* const Phone = Cast<UProjectOrionPhoneSceneComponent>(ArrayOfPhoneComponents[i]);
        if (Phone)
        {
            PhoneComponent = Phone;
            UE_LOG(LogTemp, Warning, TEXT("Found the phone component"));
        }
    }

    if (ArrayOfMotionControllers.Num() == 2)
    {
        UE_LOG(LogTemp, Warning, TEXT("FOUND 2 Motion Controllers"));
    }

	if (!LeftHandMotionController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find LEFT motion controller component."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found the LEFT motion controller component"));
        LeftHandMotionController->BeginPlay();
	}

    if (!RightHandMotionController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not find RIGHT motion controller component."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Found the motion RIGHT controller component"));
        RightHandMotionController->BeginPlay();
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectOrionCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &AProjectOrionCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AProjectOrionCharacter::MoveRight);

    InputComponent->BindAction("GrabRight", IE_Pressed, this, &AProjectOrionCharacter::GrabRight);
    InputComponent->BindAction("GrabRight", IE_Released, this, &AProjectOrionCharacter::ReleaseRight);

    InputComponent->BindAction("GrabLeft", IE_Pressed, this, &AProjectOrionCharacter::GrabLeft);
    InputComponent->BindAction("GrabLeft", IE_Released, this, &AProjectOrionCharacter::ReleaseLeft);

    InputComponent->BindAction("AnswerPhone", IE_Pressed, this, &AProjectOrionCharacter::AnswerPhone);

    InputComponent->BindAction("NextLine", IE_Pressed, this, &AProjectOrionCharacter::NextLine);
    InputComponent->BindAction("PromptUser", IE_Pressed, this, &AProjectOrionCharacter::PromptUser);

    InputComponent->BindAction("SaveDataManual", IE_Pressed, this, &AProjectOrionCharacter::SaveData);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AProjectOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AProjectOrionCharacter::LookUpAtRate);
    
}

void AProjectOrionCharacter::SaveData()
{
    SaveData("");
}

void AProjectOrionCharacter::SaveData(FString NameModifier)
{
    FString FileString = "";
    FString ReportsFolder = "../Reports/";
    FString FilePath = FPaths::GameContentDir();
    FilePath = FilePath.Append(ReportsFolder);
    FilePath = FilePath.Append(RecordDateTime.ToString());
    FilePath = FilePath.Append(NameModifier);
    FilePath = FilePath.Append(".txt");
    for (auto It = LookAtMap.CreateConstIterator(); It; ++It)
    {
        FString CurrentEntry = FString::Printf(TEXT("Actor:%s, Total Time:%f\r\n"), *It.Key(), It.Value());
        FileString = FileString.Append(CurrentEntry);
    }

    FileString = FileString.Append("\r\n\r\n");
    FString PrmoptsText = FString::Printf(TEXT("Number of prompts: %i\r\n"), NumberOfPrompts);
    FileString = FileString.Append(PrmoptsText);
    
    FDateTime EndDate = FDateTime::Now();
    FTimespan TimeSpanToFinish = EndDate - StartTime;
    FString TimeString = FString::Printf(TEXT("Time to finish: %f"), TimeSpanToFinish.GetTotalMilliseconds());
    FileString = FileString.Append("\r\n\r\n");
    FileString = FileString.Append(TimeString);

    FileString = FileString.Append("\r\n\r\n");
    FileString = FileString.Append(SavePositions());
    FileString = FileString.Append("\r\n\r\n");
   // FileString = FileString.Append(SaveRotators());
    
    FFileHelper::SaveStringToFile(FileString, *FilePath);
    UE_LOG(LogTemp, Warning, TEXT("Printing to %s"), *FilePath);
    UE_LOG(LogTemp, Warning, TEXT("%s"), *FileString);
}

FString AProjectOrionCharacter::SavePositions()
{
    FString PositionsString = "";
    for (int i = 0; i < CurrentRecordingIndex; i++)
    {
		float CurrentTime = TimeArray[i];
        FVector CurrentPosition = PositionArray[i];
		FRotator CurrentRotator = RotatorArray[i];

		FString TimeString = FString::Printf(TEXT("Time: %f "), CurrentTime);// .Append("\r\n");
		FString PositionString = CurrentPosition.ToString().Append(FString(" ") + CurrentRotator.ToString().Append("\r\n"));
		PositionsString = PositionsString.Append(TimeString);
		PositionsString = PositionsString.Append(PositionString);
	}

    return PositionsString;
}

FString AProjectOrionCharacter::SaveRotators()
{
    FString RotatorsString = "";
    for (int i = 0; i < CurrentRecordingIndex; i++)
    {
		FRotator CurrentRotator = RotatorArray[i];
        FString RotatorString = CurrentRotator.ToString().Append("\r\n");
        RotatorsString = RotatorsString.Append(RotatorString);
    }

    return RotatorsString;
}

void AProjectOrionCharacter::NextLine()
{
    PhoneComponent->SetState(PhoneState::NextAudio);
}

void AProjectOrionCharacter::PromptUser()
{
    
    if (PhoneComponent->PromptUser())
    {
        NumberOfPrompts++;
    }
}

void AProjectOrionCharacter::AnswerPhone()
{
    UE_LOG(LogTemp, Warning, TEXT("Answered the phone."));
    PhoneComponent->AnswerPhone();
}

void AProjectOrionCharacter::GrabRight()
{
    RightHandMotionController->GrabComponent();
    UE_LOG(LogTemp, Warning, TEXT("Right Grabbing"));
}

void AProjectOrionCharacter::ReleaseRight()
{
    RightHandMotionController->ReleaseComponent();
    UE_LOG(LogTemp, Warning, TEXT("Right Releasing"));
}

void AProjectOrionCharacter::GrabLeft()
{
    LeftHandMotionController->GrabComponent();
    UE_LOG(LogTemp, Warning, TEXT("Left Grabbing"));
}

void AProjectOrionCharacter::ReleaseLeft()
{
    LeftHandMotionController->ReleaseComponent();
    UE_LOG(LogTemp, Warning, TEXT("Left Releasing"));
}

void AProjectOrionCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AProjectOrionCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AProjectOrionCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AProjectOrionCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AProjectOrionCharacter::RayCastTick(UCameraComponent* Camera, float DeltaTime)
{
    FVector StartLocation = Camera->GetComponentLocation();
    FVector ScaledForwardVector = 1500.0f * Camera->GetForwardVector();

    FVector EndLocation = StartLocation + ScaledForwardVector;
    FHitResult Hit(ForceInit);
    TArray<AActor*> ActorsToIgnore;
    if (UKismetSystemLibrary::LineTraceSingle_NEW(this,
        StartLocation,
        EndLocation,
        UEngineTypes::ConvertToTraceType(ECC_Camera),
        false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, Hit, true))
    {
        //There was a hit reported.
       // UE_LOG(LogTemp, Warning, TEXT("Hit an actor: %s that far away: %f"), *Hit.Actor->GetName(), Hit.Distance);
        float& time = LookAtMap.FindOrAdd(*Hit.Actor->GetName());
        time += DeltaTime;
        LookAtMap.Add(*Hit.Actor->GetName(), time);
    }
}

void AProjectOrionCharacter::RecordingTick(float DeltaTime)
{
    if (CurrentRecordingIndex >= 54000)
    {
        // Reached maximum recording length.
        return;
    }
    FVector CurrentPosition = GetActorLocation();
    FRotator CurrentRotator = GetActorRotation();
	
	float TimeToAdd = DeltaTime;
	if (CurrentRecordingIndex != 0)
	{
		TimeToAdd += TimeArray[CurrentRecordingIndex - 1];
	}
	TimeArray[CurrentRecordingIndex] = TimeToAdd;
    PositionArray[CurrentRecordingIndex] = CurrentPosition;
    RotatorArray[CurrentRecordingIndex] = CurrentRotator;

    CurrentRecordingIndex++;
}

void AProjectOrionCharacter::RadioTouched()
{
    UE_LOG(LogTemp, Warning, TEXT("The radio player is touched. Should now play the thank you audio."));
    PhoneComponent->SetState(PhoneState::CabinetDrawer);
}

void AProjectOrionCharacter::GrabbedWallet()
{
    SaveData("GrabbedWallet");
}

FVector AProjectOrionCharacter::CalculateCameraPosition(FRotator CameraRotator, UCameraComponent* Camera)
{
    float CurrentRadius = BaseRadius * BaseRadiusScale;

    // Rotator uses degrees.
    float CameraPitch = CameraRotator.Pitch;


    UE_LOG(LogTemp, Warning, TEXT("Camera rotation is: %s"), *CameraRotator.ToString());


    //At 0 it should be at the top of the circle, and as it goes down, it goes around the circle, up should have no effect.
    //Pitch is clamped between 0 and - 100 for the calculation.
    CameraPitch = UKismetMathLibrary::DegreesToRadians(UKismetMathLibrary::ClampAngle(-CameraPitch, 0.0f, 100.0f));
    
    float NewX = CurrentRadius * sin(CameraPitch);
    float NewZ = CurrentRadius * cos(CameraPitch);

	FVector ModifiedForward = Camera->GetForwardVector();
	ModifiedForward.Z = 0.0f;
	ModifiedForward *= NewX;

    UE_LOG(LogTemp, Warning, TEXT("New X: %f New Z: %f"), NewX, NewZ);
    return FVector(0.0f, 0.0f, NewZ) - ModifiedForward;
}