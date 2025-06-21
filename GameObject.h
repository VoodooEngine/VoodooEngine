#pragma once

// Game object 
//---------------------
// This class is used as a base class for all objects placed in levels, 
// if a game object needs custom stuff e.g. an object with more than a single bitmap/collision etc. 
// then that object would be derived as a child from this class)
//---------------------
class GameObject : public Object
{
public:
	BitmapComponent GameObjectBitmap;
	SVector GameObjectDimensions = { 0, 0 };
	bool GameObjectBitmapHiddenInGame = false;

	// Never account for negative value as game object ID as that is the default value
	int GameObjectID = -1;

	// Optional, can be set to not be created
	CollisionComponent DefaultGameObjectCollision;
	bool CreateDefaultGameObjectCollisionInGame = false;

	// Optional custom constructor, called after everything has been initialized for the game object
	virtual void OnGameObjectCreated(SVector SpawnLocation){};

	// Optional custom deconstructor, called right before an object is set to be deleted, 
	// can be used to delete additional stuff created outside of this class in their own class
	// e.g. a player class where additional bitmaps/collision has been created inside player class 
	// that needs to be deleted
	virtual void OnGameObjectDeleted(){};

	// Enable/disable bitmap rendering/default object collision
	virtual void UpdateGameObjectState(bool Enable = false)
	{
		if (Enable)
		{
			if (!GameObjectBitmapHiddenInGame)
			{
				GameObjectBitmap.BitmapParams.BitmapSetToNotRender = false;
			}

			// Set no collision as default, and if set to have collision enable it
			DefaultGameObjectCollision.NoCollision = true;
			if (CreateDefaultGameObjectCollisionInGame)
			{
				DefaultGameObjectCollision.NoCollision = false;
			}
		}
		else if (!Enable)
		{
			GameObjectBitmap.BitmapParams.BitmapSetToNotRender = true;
			DefaultGameObjectCollision.NoCollision = true;
		}
	}

	// These can be used by any instance of the game object class
	virtual void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag, Object* Target = nullptr){};
	virtual void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag){};
};
