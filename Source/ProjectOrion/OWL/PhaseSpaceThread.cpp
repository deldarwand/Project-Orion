#include "ProjectOrion.h"
#include "PhaseSpaceThread.h"

PhaseSpaceThread* PhaseSpaceThread::PSThread = NULL;

PhaseSpaceThread::PhaseSpaceThread()
{
	Thread = FRunnableThread::Create(this, TEXT("PhaseSpace"), false);
	LeftFootID = 2;
	RightFootID = 60;
	CanAccessMarkers = new FCriticalSection();
	ShouldShutDown = false;
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
						CanAccessMarkers->Lock();
						if (m->id == LeftFootID)
						{
							LeftLegPosition.X = m->x;
							LeftLegPosition.Y = m->y;
							LeftLegPosition.Z = m->z;
						}
						if (m->id == RightFootID)
						{
							RightLegPosition.X = m->x;
							RightLegPosition.Y = m->y;
							RightLegPosition.Z = m->z;
						}
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
