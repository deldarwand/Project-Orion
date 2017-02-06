// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectOrion.h"
#include "ProjectOrionCharacter.h"
#include "ProjectOrionProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Door.h"
#include "ProjectOrionMotionController.h"

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
    UE_LOG(LogTemp, Warning, TEXT("Function to calculate camera within Project Orion Character"));
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
     /*   if (MotionController->Hand == EControllerHand::Left) LeftHandMotionController = MotionController;
        if (MotionController->Hand == EControllerHand::Right) RightHandMotionController = MotionController;
        if (MotionController->Hand == EControllerHand::Pad) UE_LOG(LogTemp, Warning, TEXT("DIE"));*/
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
	}

    if (!RightHandMotionController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not find RIGHT motion controller component."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Found the motion RIGHT controller component"));
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

	//Add the interaction keypressed
	InputComponent->BindAction("Interact", IE_Pressed, this, &AProjectOrionCharacter::SceneInteract);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AProjectOrionCharacter::TouchStarted);
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AProjectOrionCharacter::OnFire);
	}

	InputComponent->BindAxis("MoveForward", this, &AProjectOrionCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AProjectOrionCharacter::MoveRight);

	// Binds all of the motion controller toggles to the functions that will allow the movement and rotation to be set.
	InputComponent->BindAction("MoveMotionControllerLeft", IE_Pressed, this, &AProjectOrionCharacter::ToggleMotionLeft);
	InputComponent->BindAction("MoveMotionControllerLeft", IE_Released, this, &AProjectOrionCharacter::ToggleMotionLeft);

	InputComponent->BindAction("MoveMotionControllerRight", IE_Pressed, this, &AProjectOrionCharacter::ToggleMotionRight);
	InputComponent->BindAction("MoveMotionControllerRight", IE_Released, this, &AProjectOrionCharacter::ToggleMotionRight);

	InputComponent->BindAction("MoveMotionControllerForward", IE_Pressed, this, &AProjectOrionCharacter::ToggleMotionForward);
	InputComponent->BindAction("MoveMotionControllerForward", IE_Released, this, &AProjectOrionCharacter::ToggleMotionForward);

	InputComponent->BindAction("MoveMotionControllerBack", IE_Pressed, this, &AProjectOrionCharacter::ToggleMotionBack);
	InputComponent->BindAction("MoveMotionControllerBack", IE_Released, this, &AProjectOrionCharacter::ToggleMotionBack);

	InputComponent->BindAction("MoveMotionControllerUp", IE_Pressed, this, &AProjectOrionCharacter::ToggleMotionUp);
	InputComponent->BindAction("MoveMotionControllerUp", IE_Released, this, &AProjectOrionCharacter::ToggleMotionUp);

	InputComponent->BindAction("MoveMotionControllerDown", IE_Pressed, this, &AProjectOrionCharacter::ToggleMotionDown);
	InputComponent->BindAction("MoveMotionControllerDown", IE_Released, this, &AProjectOrionCharacter::ToggleMotionDown);

	InputComponent->BindAction("RotateControllerAroundX", IE_Pressed, this, &AProjectOrionCharacter::ToggleXRotation);
	InputComponent->BindAction("RotateControllerAroundX", IE_Released, this, &AProjectOrionCharacter::ToggleXRotation);

	InputComponent->BindAction("RotateControllerAroundY", IE_Pressed, this, &AProjectOrionCharacter::ToggleYRotation);
	InputComponent->BindAction("RotateControllerAroundY", IE_Released, this, &AProjectOrionCharacter::ToggleYRotation);

	InputComponent->BindAction("RotateControllerAroundZ", IE_Pressed, this, &AProjectOrionCharacter::ToggleZRotation);
	InputComponent->BindAction("RotateControllerAroundZ", IE_Released, this, &AProjectOrionCharacter::ToggleZRotation);

    InputComponent->BindAction("GrabRight", IE_Pressed, this, &AProjectOrionCharacter::GrabRight);
    InputComponent->BindAction("GrabRight", IE_Released, this, &AProjectOrionCharacter::ReleaseRight);

    InputComponent->BindAction("GrabLeft", IE_Pressed, this, &AProjectOrionCharacter::GrabLeft);
    InputComponent->BindAction("GrabLeft", IE_Released, this, &AProjectOrionCharacter::ReleaseLeft);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AProjectOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AProjectOrionCharacter::LookUpAtRate);
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

void AProjectOrionCharacter::ToggleXRotation()
{
	LeftHandMotionController->ToggleXRotation();
}

void AProjectOrionCharacter::ToggleYRotation()
{
	LeftHandMotionController->ToggleYRotation();
}

void AProjectOrionCharacter::ToggleZRotation()
{
	LeftHandMotionController->ToggleZRotation();
}

void AProjectOrionCharacter::ToggleMotionLeft()
{
	LeftHandMotionController->ToggleMoveLeft();
}

void AProjectOrionCharacter::ToggleMotionRight()
{
	LeftHandMotionController->ToggleMoveRight();
}

void AProjectOrionCharacter::ToggleMotionForward()
{
	LeftHandMotionController->ToggleMoveForward();
}

void AProjectOrionCharacter::ToggleMotionBack()
{
	LeftHandMotionController->ToggleMoveBack();
}

void AProjectOrionCharacter::ToggleMotionUp()
{
	LeftHandMotionController->ToggleMoveUp();
}

void AProjectOrionCharacter::ToggleMotionDown()
{
	LeftHandMotionController->ToggleMoveDown();
}

void AProjectOrionCharacter::SceneInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("You pressed the interact button"));
	
	TArray<AActor*> InteractableActors;
	InteractionSphere->GetOverlappingActors(InteractableActors);

	UE_LOG(LogTemp, Warning, TEXT("The number of intersected objects is : %i"), InteractableActors.Num());

	for (int i = 0; i < InteractableActors.Num(); i++)
	{
		ADoor* const currentDoor = Cast<ADoor>(InteractableActors[i]);
		if (currentDoor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Went into door"));
			if (!currentDoor->IsPendingKill())
			{
				currentDoor->Destroy();
			}
		}
	}
}

void AProjectOrionCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AProjectOrionProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

}

void AProjectOrionCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AProjectOrionCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AProjectOrionCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
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

bool AProjectOrionCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AProjectOrionCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AProjectOrionCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AProjectOrionCharacter::TouchUpdate);
	}
	return bResult;
}
