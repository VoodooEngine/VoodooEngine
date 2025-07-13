#pragma once

// Character class that can be used as a base for a player or NPC ect., has built in update component.
// Note that "OnGameObjectCreated" and OnGameObjectDeleted" is called in character class, 
// since they are used internally by the character class to add and remove update component.
// But if you still want to add functionality for when a character is created/deleted on a child derived class, 
// then make sure to call the parent created/deleted function so we add and remove update component 
class Character : public GameObject, public UpdateComponent
{
public:
	MovementComponent MoveComp;

	void OnGameObjectCreated(SVector SpawnLocation)
	{
		VoodooEngine::Engine->StoredUpdateComponents.push_back(this);
	}
	void OnGameObjectDeleted()
	{
		VoodooEngine::Engine->RemoveComponent(
			(UpdateComponent*)this, &VoodooEngine::Engine->StoredUpdateComponents);
	}
};
