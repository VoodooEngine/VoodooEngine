#pragma once

#include "VoodooEngineDLLExport.h"
#include "TransformComponent.h"
#include "Vector.h"

// Object
//---------------------
// Base object class that is the parent class to the game object class,
// has internal begin/end overlap events for when a collision is detected
//---------------------
class Object
{
public:
	SVector Location;

	// These are overriden by the game object class
	virtual void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag, Object* Target = nullptr){};
	virtual void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag){};
};

extern "C" VOODOOENGINE_API void SetObjectLocation(Object* Object, SVector NewLocation);
extern "C" VOODOOENGINE_API SVector GetObjectLocation(Object* Object);

// This will set the location relative to the owner of the component 
// e.g. a player is the owner object of the component, 
// a gun is the component that will have its location set relative to where the player is, in local space
// so if we set the player position in screen space to X=100, Y=200, and we set the gun position to be X=10, Y=20,
// the gun position will end up being X=100 + X=10, Y=200 + Y=20
extern "C" VOODOOENGINE_API void SetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component, SVector NewLocation);

// Get the relative location of an object relative to component owner
// (The result of the addition of the owner object, e.g. player location 
// and the component e.g. player's gun location)
extern "C" VOODOOENGINE_API SVector GetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component);
