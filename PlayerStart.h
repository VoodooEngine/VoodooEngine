#pragma once

#include "GameObject.h"

class PlayerStart : public GameObject
{
public:
	void SetVisibility(bool Show)
	{
		if (Show)
		{
			GameObjectBitmap.BitmapParams.BitmapSetToNotRender = false;
			DefaultGameObjectCollision.RenderCollisionRect = true;
		}
		else
		{
			GameObjectBitmap.BitmapParams.BitmapSetToNotRender = true;
			DefaultGameObjectCollision.RenderCollisionRect = false;
		}
	}
};
