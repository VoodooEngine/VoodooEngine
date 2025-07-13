#include "CollisionComponent.h"

bool IsCollisionDetected(CollisionComponent* Sender, CollisionComponent* Target)
{
	if (Sender->NoCollision ||
		Target->NoCollision ||
		Sender == Target)
	{
		return false;
	}

	for (int i = 0; i < Sender->CollisionTagsToIgnore.size(); ++i)
	{
		if (Sender->CollisionTagsToIgnore[i] == Target->CollisionTag)
		{
			return false;
		}
	}

	if (Sender->ComponentLocation.X < Target->ComponentLocation.X + Target->CollisionRect.X &&
		Target->ComponentLocation.X < Sender->ComponentLocation.X + Sender->CollisionRect.X &&
		Sender->ComponentLocation.Y < Target->ComponentLocation.Y + Target->CollisionRect.Y &&
		Target->ComponentLocation.Y < Sender->ComponentLocation.Y + Sender->CollisionRect.Y)
	{
		return true;
	}

	return false;
}

void BroadcastCollision(Object* CallbackOwner, CollisionComponent* Sender, CollisionComponent* Target)
{
	if (!Sender)
	{
		return;
	}
	if (!Target)
	{
		return;
	}
	if (Sender->NoCollision)
	{
		return;
	}
	if (Target->NoCollision)
	{
		return;
	}
	bool Ignore = false;
	if (IsCollisionDetected(Sender, Target))
	{
		for (int i = 0; i < Sender->CollisionTagsToIgnore.size(); ++i)
		{
			if (Target->CollisionTag == Sender->CollisionTagsToIgnore[i])
			{
				Ignore = true;
				break;
			}
		}

		if (!Ignore &&
			!Sender->IsOverlapped)
		{
			Sender->IsOverlapped = true;
			CallbackOwner->OnBeginOverlap(Sender->CollisionTag, Target->CollisionTag, Target->Owner);
		}
	}
	else if (!Ignore &&
		Sender->IsOverlapped)
	{
		Sender->IsOverlapped = false;
		CallbackOwner->OnEndOverlap(Sender->CollisionTag, Target->CollisionTag);
	}
}
