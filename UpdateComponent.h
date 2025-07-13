#pragma once

// Update component inherited by all objects that needs to update each frame
class UpdateComponent
{
public:
	bool Paused = false;
	virtual void Update(float DeltaTime) = 0;
};
