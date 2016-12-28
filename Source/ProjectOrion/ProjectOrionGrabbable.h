// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectOrionGrabbable.generated.h"

UCLASS()
class PROJECTORION_API AProjectOrionGrabbable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectOrionGrabbable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    void GrabbedBy(class USceneComponent* componentToAttachTo);
    void ReleasedBy(class USceneComponent* componentToAttachTo);

    

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grabbable", meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* GrabMesh;
};
