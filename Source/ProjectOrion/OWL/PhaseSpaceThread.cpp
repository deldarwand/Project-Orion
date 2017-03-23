#include "ProjectOrion.h"
#include "PhaseSpaceThread.h"
#include "Kismet/KismetMathLibrary.h"

PhaseSpaceThread* PhaseSpaceThread::PSThread = NULL;

PhaseSpaceThread::PhaseSpaceThread()
{
	Thread = FRunnableThread::Create(this, TEXT("PhaseSpace"), false);
	LeftFootIDs.push_back(0);
	LeftFootIDs.push_back(2);
	RightFootIDs.push_back(61);
	RightFootIDs.push_back(64);
	CanAccessMarkers = new FCriticalSection();
	ShouldShutDown = false;
	NumberOfMarkersPerFoot = 2;
	LeftLegPositions.push_back(FVector::ZeroVector);
	LeftLegPositions.push_back(FVector::ZeroVector);
	RightLegPositions.push_back(FVector::ZeroVector);
	RightLegPositions.push_back(FVector::ZeroVector);
	FoundPhaseSpace = false;
}

bool PhaseSpaceThread::Init()
{
	return true;
}

void PhaseSpaceThread::Stop()
{
	MarkForShutdown();
}

PhaseSpaceThread* PhaseSpaceThread::StartPhaseSpace()
{
	if (!PSThread && FPlatformProcess::SupportsMultithreading())
	{
		PSThread = new PhaseSpaceThread();
	}
	return PSThread;
}

void PhaseSpaceThread::MarkForShutdown()
{
	ShouldShutDown = true;
}

uint32 PhaseSpaceThread::Run()
{
	std::string address = "128.16.8.253";
	
	if (Owl.open(address) <= 0 || Owl.initialize() <= 0) return 1;

    FoundPhaseSpace = true;
	// start streaming
	Owl.streaming(1);

	// main loop
	while (Owl.isOpen() && Owl.property<int>("initialized") && !ShouldShutDown)
	{
		const OWL::Event *event = Owl.nextEvent(1000);
		if (!event) continue;

		if (event->type_id() == OWL::Type::ERROR)
		{
			break;
		}
		else if (event->type_id() == OWL::Type::FRAME)
		{
			if (event->find("markers", Markers) > 0)
			{
				for (OWL::Markers::iterator m = Markers.begin(); m != Markers.end(); m++)
					if (m->cond > 0)
					{
						CanAccessMarkers->Lock();
						if (m->id == LeftFootIDs[0])
						{
							LeftLegPosition.X = m->x;
							LeftLegPosition.Y = m->y;
							LeftLegPosition.Z = m->z;
						}
						if (m->id == LeftFootIDs[1])
						{
							LeftLegPositions[1].X = m->x;
							LeftLegPositions[1].Y = m->y;
							LeftLegPositions[1].Z = m->z;
						}
						if (m->id == RightFootIDs[0])
						{
							RightLegPosition.X = m->x;
							RightLegPosition.Y = m->y;
							RightLegPosition.Z = m->z;
						}
						if (m->id == RightFootIDs[1])
						{
							RightLegPositions[1].X = m->x;
							RightLegPositions[1].Y = m->y;
							RightLegPositions[1].Z = m->z;
						}
						FVector TempLeftLeg = (LeftLegPositions[0] + LeftLegPositions[1]) / 2.0f;
						FVector TempRightLeg = (RightLegPositions[0] + RightLegPositions[1]) / 2.0f;

						FRotator TempLeftLegRot = (LeftLegPositions[0] - LeftLegPositions[1]).Rotation();
						FRotator TempRightLegRot = (RightLegPositions[0] - RightLegPositions[1]).Rotation();

						
				//		LeftLegPosition = LeftLegPositions[0];
				//		RightLegPosition = RightLegPositions[0];
				//		LeftLegRotation = TempLeftLegRot;
				//		RightLegRotation = TempRightLegRot;
						CanAccessMarkers->Unlock();
					}
			}
		}
	} // while
	Owl.done();
	Owl.close();
	return 0;
}

PhaseSpaceThread::~PhaseSpaceThread()
{
	delete Thread;
	delete CanAccessMarkers;
	Thread = NULL;
}
