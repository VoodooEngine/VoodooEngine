#pragma once

#include "VoodooEngine.h"

class Trigger : public GameObject, public UpdateComponent, public IGameState
{
public:
	std::vector<CollisionComponent*> CollisionTargets;

	void OnGameObjectCreated(SVector SpawnLocation)
	{
		VoodooEngine::Engine->StoredUpdateComponents.push_back(this);
		VoodooEngine::Engine->InterfaceObjects_GameState.push_back(this);
	}
	void OnGameObjectDeleted()
	{
		VoodooEngine::Engine->RemoveComponent(
			(UpdateComponent*)this, &VoodooEngine::Engine->StoredUpdateComponents);
		VoodooEngine::Engine->RemoveComponent(
			(IGameState*)this, &VoodooEngine::Engine->InterfaceObjects_GameState);
	}

	void InterfaceEvent_GameStart()
	{
		if (!VoodooEngine::Engine->DebugMode)
		{
			GameObjectBitmap.BitmapParams.BitmapSetToNotRender = true;
		}
	}
	void InterfaceEvent_GameEnd()
	{
		GameObjectBitmap.BitmapParams.BitmapSetToNotRender = false;
	}

	void Update(float DeltaTime)
	{
		for (int i = 0; i < CollisionTargets.size(); ++i)
		{
			BroadcastCollision(this, &DefaultGameObjectCollision, CollisionTargets[i]);
		}
	}

	// Optional to override the collision parameters 
	// (by default the collision rect is the same size as the "GameObjectBitmap" for the trigger)
	void SetTriggerParameters(int CollisionTag, SVector TriggerBoxSize)
	{
		DefaultGameObjectCollision.CollisionType = ECollisionType::Collision_Overlap;
		DefaultGameObjectCollision.CollisionRect = TriggerBoxSize;
		DefaultGameObjectCollision.CollisionRectColor = VoodooEngine::Engine->ColorYellow;
		DefaultGameObjectCollision.CollisionTag = CollisionTag;
		DefaultGameObjectCollision.Owner = this;

		if (VoodooEngine::Engine->DebugMode)
		{
			DefaultGameObjectCollision.RenderCollisionRect = true;
		}
	}

	void SetTriggerLocation(SVector NewLocation)
	{
		Location = NewLocation;
		GameObjectBitmap.ComponentLocation = NewLocation;
		DefaultGameObjectCollision.ComponentLocation = NewLocation;
	}

	// These are called by the "BroadcastCollision" function
	void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag, Object* Target = nullptr) {};
	void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag) {};
};
