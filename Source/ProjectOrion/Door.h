// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class PROJECTORION_API ADoor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ADoor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Return the mesh for the pickup */
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return DoorMesh; }

private:
	/** Static mesh for the door in the level*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DoorMesh;
	
};
