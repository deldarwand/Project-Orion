#pragma once
#include "ProjectOrion.h"
#include "PhaseSpace/owl.hpp"

class PhaseSpaceThread : public FRunnable
{
public:
	PhaseSpaceThread();
	~PhaseSpaceThread();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	static PhaseSpaceThread* StartPhaseSpace();

	void MarkForShutdown();

	FCriticalSection* CanAccessMarkers;
	FVector LeftLegPosition;
	FVector RightLegPosition;
	FRunnableThread* Thread;
	static PhaseSpaceThread* PSThread;

	uint32 LeftFootID;
	uint32 RightFootID;

	OWL::Context Owl;
	OWL::Markers Markers;

private:
	bool ShouldShutDown;

};

