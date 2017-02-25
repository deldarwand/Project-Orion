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

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = true;
	FP_Gun->CastShadow = true;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));



	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	ActorMesh->SetupAttachment(FirstPersonCameraComponent);
	ActorMesh->RelativeLocation = FVector(0.0f, 0.0f, 0.0f);

	//Add the interaction sphere to the character
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	//InteractionSphere->AttachTo(RootComponent);
	InteractionSphere->SetSphereRadius(200.0f);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AProjectOrionCharacter::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
    //UE_LOG(LogTemp, Warning, TEXT("Function to calculate camera within Project Orion Character"));
    AActor::CalcCamera(DeltaTime, OutResult);
    //OutResult.Location += FVector(0.0f, 0.0f, 50.0f);
}

void AProjectOrionCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
	
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

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AProjectOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AProjectOrionCharacter::LookUpAtRate);
    
}

void AProjectOrionCharacter::NextLine()
{
    PhoneComponent->SetState(PhoneState::NextAudio);
}

void AProjectOrionCharacter::PromptUser()
{
    PhoneComponent->PromptUser();
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

void AProjectOrionCharacter::RayCastTick(UCameraComponent* Camera)
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
    }
    

}

void AProjectOrionCharacter::RadioTouched()
{
    UE_LOG(LogTemp, Warning, TEXT("The radio player is touched. Should now play the thank you audio."));
    PhoneComponent->SetState(PhoneState::CabinetDrawer);
}