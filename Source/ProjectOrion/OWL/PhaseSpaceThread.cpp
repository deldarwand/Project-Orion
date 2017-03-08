#include "ProjectOrion.h"
#include "PhaseSpaceThread.h"

PhaseSpaceThread* PhaseSpaceThread::PSThread = NULL;

PhaseSpaceThread::PhaseSpaceThread()
{
	Thread = FRunnableThread::Create(this, TEXT("PhaseSpace"), false);
	LeftFootIDs.push_back(2);
	LeftFootIDs.push_back(4);
	RightFootIDs.push_back(60);
	RightFootIDs.push_back(61);
	CanAccessMarkers = new FCriticalSection();
	ShouldShutDown = false;
	NumberOfMarkersPerFoot = 2;
	LeftLegPositions.reserve(NumberOfMarkersPerFoot);
	RightLegPositions.reserve(NumberOfMarkersPerFoot);
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

	// start streaming
	Owl.streaming(1);

	// main loop
	while (Owl.isOpen() && Owl.property<int>("initialized") && !ShouldShutDown)
	{
		const OWL::Event *event = Owl.nextEvent(1000);
		if (!event) continue;

		if (event->type_id() == OWL::Type::ERROR)
		{
			//cerr << event->name() << ": " << event->str() << endl;
			break;
		}
		else if (event->type_id() == OWL::Type::FRAME)
		{
			//cout << "time=" << event->time() << " " << event->type_name() << " " << event->name() << "=" << event->size<OWL::Event>() << ":" << endl;
			if (event->find("markers", Markers) > 0)
			{
				//cout << " markers=" << markers.size() << ":" << endl;
				for (OWL::Markers::iterator m = Markers.begin(); m != Markers.end(); m++)
					if (m->cond > 0)
					{
						
						if (m->id == LeftFootIDs[0])
						{
							LeftLegPositions[0].X = m->x;
							LeftLegPositions[0].Y = m->y;
							LeftLegPositions[0].Z = m->z;
						}
						if (m->id == LeftFootIDs[1])
						{
							LeftLegPositions[1].X = m->x;
							LeftLegPositions[1].Y = m->y;
							LeftLegPositions[1].Z = m->z;
						}
						if (m->id == RightFootIDs[0])
						{
							RightLegPositions[0].X = m->x;
							RightLegPositions[0].Y = m->y;
							RightLegPositions[0].Z = m->z;
						}
						if (m->id == RightFootIDs[1])
						{
							RightLegPositions[1].X = m->x;
							RightLegPositions[1].Y = m->y;
							RightLegPositions[1].Z = m->z;
						}
						FVector TempLeftLeg = (LeftLegPositions[0] + LeftLegPositions[0]) / 2.0f;
						FVector TempRightLeg = (RightLegPositions[0] + RightLegPositions[0]) / 2.0f;
						CanAccessMarkers->Lock();
						LeftLegPosition = TempLeftLeg;
						RightLegPosition = TempRightLeg;
						CanAccessMarkers->Unlock();
					}
						//cout << "  " << m->id << ") " << m->x << "," << m->y << "," << m->z << endl;
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
