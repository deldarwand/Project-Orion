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

	FRotator LeftLegRotation;
	FRotator RightLegRotation;

	FRunnableThread* Thread;
	static PhaseSpaceThread* PSThread;

	std::vector<uint32> LeftFootIDs;
	std::vector<uint32> RightFootIDs;

	OWL::Context Owl;
	OWL::Markers Markers;

private:
	bool ShouldShutDown;
	int NumberOfMarkersPerFoot;
	std::vector<FVector> LeftLegPositions;
	std::vector<FVector> RightLegPositions;

	FRotator CalculateRotationBetweenDifference(FVector DifferenceVector);
};

