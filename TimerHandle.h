#pragma once

#include "UpdateComponent.h"
#include "VoodooEngine.h"

// Timer that counts down and sends a function pointer callback when finished
class TimerHandle : public UpdateComponent
{
public:
	float TimerValue = 1;
	bool TimerCompleted = false;
	void(*OnTimerEndFunctionPointer)(void);

	void SetTimer(float NewTime)
	{
		VoodooEngine::Engine->StoredTimerUpdateComponents.push_back(this);
		TimerCompleted = false;
		TimerValue = NewTime;
	};

	void Update(float DeltaTime)
	{
		TimerValue -= 0.025;
		if (TimerValue <= 0)
		{
			if (!TimerCompleted)
			{
				TimerCompleted = true;
				OnTimerEndFunctionPointer();
				VoodooEngine::Engine->RemoveComponent(
					(UpdateComponent*)this, &VoodooEngine::Engine->StoredTimerUpdateComponents);
			}
		}
	};
};
