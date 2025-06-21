#pragma once

#include "VoodooEngineDLLExport.h"
#include "TransformComponent.h"
#include "Object.h"
#include "SColor.h"
#include <vector>

// Should collision block or overlap
enum ECollisionType
{
	Collision_Block,
	Collision_Overlap
};

// Collision
//---------------------
// Collision component class, 
// contains all the needed collision parameters, 
// inherited by all objects that needs collision detection
class CollisionComponent : public TransformComponent
{
public:
	ECollisionType CollisionType = ECollisionType::Collision_Block;
	bool NoCollision = false;
	bool IsOverlapped = false;
	bool RenderCollisionRect = false;
	bool DrawFilledRectangle = false;
	float Opacity = 1;
	int CollisionTag = -1;
	std::vector<int> CollisionTagsToIgnore;
	SColor CollisionRectColor;
	SVector CollisionRect;
	SVector CollisionRectOffset;
	Object* Owner = nullptr;
};

extern "C" VOODOOENGINE_API bool IsCollisionDetected(
	CollisionComponent* Sender, CollisionComponent* Target);
extern "C" VOODOOENGINE_API void BroadcastCollision(
	Object* CallbackOwner, CollisionComponent* Sender, CollisionComponent* Target);
