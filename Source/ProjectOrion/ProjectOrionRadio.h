// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectOrionRadio.generated.h"

UCLASS()
class PROJECTORION_API AProjectOrionRadio : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectOrionRadio();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio Audio")
    class UAudioComponent* RadioAudioComponent;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio Audio")
    class USoundBase* AudioComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio Audio")
    class UCapsuleComponent* RadioInteractionCapsule;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio Mesh")
    class UStaticMeshComponent* RadioStaticMesh;

    

};
