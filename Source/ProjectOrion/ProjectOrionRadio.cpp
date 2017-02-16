// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionRadio.h"
#include "ProjectOrionCharacter.h"

// Sets default values
AProjectOrionRadio::AProjectOrionRadio()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    RadioAudioComponent = CreateDefaultSubobject<UAudioComponent>("RadioAudioComponent");

    if (AudioComponent)
    {
        RadioAudioComponent->SetSound(AudioComponent);
    }

    RadioInteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>("RadioInteractionCapsule");
    RadioInteractionCapsule->SetCapsuleRadius(25.0f);

    RadioStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("RadioStaticMesh");

}

// Called when the game starts or when spawned
void AProjectOrionRadio::BeginPlay()
{
	Super::BeginPlay();
    //Start the radio audio playing at 30 seconds in.
    RadioAudioComponent->Play(30.0f);
}

// Called every frame
void AProjectOrionRadio::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    TArray<AActor*> OverlappingActors;
    RadioInteractionCapsule->GetOverlappingActors(OverlappingActors);

    UE_LOG(LogTemp, Warning, TEXT("The number of intersected objects is : %i"), OverlappingActors.Num());

    for (int i = 0; i < OverlappingActors.Num(); i++)
    {
        AProjectOrionCharacter* const ProjectOrionCharacter = Cast<AProjectOrionCharacter>(OverlappingActors[i]);
        if (ProjectOrionCharacter)
        {
            UE_LOG(LogTemp, Warning, TEXT("Touching the radio"));
            RadioAudioComponent->Stop();
        }
    }
}

