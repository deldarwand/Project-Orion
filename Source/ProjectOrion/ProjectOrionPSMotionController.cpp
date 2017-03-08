// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOrion.h"
#include "ProjectOrionPSMotionController.h"
#include "OWL/PhaseSpaceTracker.hpp"
#include "OWL/PhaseSpaceThread.h"

UProjectOrionPSMotionController::UProjectOrionPSMotionController()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;


	/*std::string address = "128.16.8.253";
	OWL::Context owl;
	OWL::Markers markers;
	if (owl.open(address) <= 0 || owl.initialize() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FAIL"));
	}

	// start streaming
	owl.streaming(1);
	
	// main loop
	while (owl.isOpen() && owl.property<int>("initialized"))
	{
		const OWL::Event *event = owl.nextEvent(1000);
		if (!event) continue;

		if (event->type_id() == OWL::Type::ERROR)
		{
			//cerr << event->name() << ": " << event->str() << endl;
			break;
		}
		else if (event->type_id() == OWL::Type::FRAME)
		{
			//cout << "time=" << event->time() << " " << event->type_name() << " " << event->name() << "=" << event->size<OWL::Event>() << ":" << endl;
			if (event->find("markers", markers) > 0)
			{
				//cout << " markers=" << markers.size() << ":" << endl;
				for (OWL::Markers::iterator m = markers.begin(); m != markers.end(); m++)
					if (m->cond > 0)
					{
						FVector Position = FVector(m->x, m->y, m->z);
						UE_LOG(LogTemp, Warning, TEXT("Got location %s"), *Position.ToString());
					}
					//	cout << "  " << m->id << ") " << m->x << "," << m->y << "," << m->z << endl;
			}
		}
	} // while
	owl.done();
	owl.close();*/

    PlayerIndex = 0;
    bDisableLowLatencyUpdate = false;
	PhaseSpaceThreadInstance = PhaseSpaceThread::StartPhaseSpace();
}

void UProjectOrionPSMotionController::BeginPlay()
{

}

void UProjectOrionPSMotionController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	FVector Position;
	FRotator Orientation;

	PhaseSpaceThreadInstance->CanAccessMarkers->Lock();
	if (Hand == EControllerHand::Right)
	{
		Position = ConvertFromPSToUE(PhaseSpaceThreadInstance->RightLegPosition);
	}
	else if (Hand == EControllerHand::Left)
	{
		Position = ConvertFromPSToUE(PhaseSpaceThreadInstance->LeftLegPosition);
	}
	PhaseSpaceThreadInstance->CanAccessMarkers->Unlock();

	bool bTracked = true;//PollControllerState(Position, Orientation);
    if (bTracked)
    {
		if (Hand == EControllerHand::Left)
		{
			UE_LOG(LogTemp, Warning, TEXT("Got LEFT location %s"), *Position.ToString());
		}
		else if (Hand == EControllerHand::Right)
		{
			UE_LOG(LogTemp, Warning, TEXT("Got RIGHT location %s"), *Position.ToString());
		}
		SetRelativeLocationAndRotation(Position, Orientation);
    }
}

bool UProjectOrionPSMotionController::PollControllerState(FVector& Position, FRotator& Orientation)
{
    // Use the PhaseSpace library to get position and rotation for the controller.
    Position = GetComponentLocation();
    Orientation = GetComponentRotation();

	std::vector<float> footLocation;
	if (Hand == EControllerHand::Left)
	{
		footLocation = LegTracker->GetLeftFoot();
	}
	else if (Hand == EControllerHand::Right)
	{
		footLocation = LegTracker->GetRightFoot();
	}
	FVector footLocationV = FVector(footLocation[0], footLocation[1], footLocation[2]);
	//footLocationV = ConvertFromPSToUE(footLocationV);
	
	Position = footLocationV;

    return true;
}

FVector UProjectOrionPSMotionController::ConvertFromPSToUE(FVector PSPosition)
{
	FVector UEPosition;

	for (int i = 0; i < 3; i++)
	{
		UEPosition[i] = (PSPosition[i] / 1000.0f);
	}
	return UEPosition;
}