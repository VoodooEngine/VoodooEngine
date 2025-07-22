#pragma once

// Character class that can be used as a base for a player or NPC ect., 
// derived from "GameObject" class and has built in update component.
// 
// NOTE: "Update", "OnGameObjectCreated" and OnGameObjectDeleted" is called in character class, 
// since they are used internally by the character class.
// But if you still want to add functionality for when a character is created/deleted/update in a child derived class, 
// then make sure to call the parent update/created/deleted functions so we keep the functionality of the parent class
// 
// ADDITIONAL NOTE:
// An additional "GameObjectFlippedBitmap" is auto created/deleted,
// will also auto update its location to be the same as the default "GameObjectBitmap", 
// also auto hide/show the appropriate bitmap based of moving left or right 
class Character : public GameObject, public UpdateComponent
{
public:
	BitmapComponent GameObjectFlippedBitmap;
	MovementComponent MoveComp;

	/*
	void CreateFlippedBitmap()
	{
		auto Iterator = VoodooEngine::Engine->StoredGameObjectIDs.find(GameObjectID);
		const wchar_t* FilePath = Iterator->second.AssetFilePath.c_str();

		GameObjectFlippedBitmap.Bitmap = SetupBitmap(
			GameObjectFlippedBitmap.Bitmap, 
			FilePath,
			VoodooEngine::Engine->Renderer, 
			true);
		SetupBitmapComponent(&GameObjectFlippedBitmap, GameObjectFlippedBitmap.Bitmap);
		GameObjectFlippedBitmap.BitmapParams = GameObjectBitmap.BitmapParams;
		GameObjectFlippedBitmap.BitmapParams.BitmapSetToNotRender = true;
		VoodooEngine::Engine->StoredBitmapComponents.push_back(&GameObjectFlippedBitmap);
	}
	*/

	void OnGameObjectCreated(SVector SpawnLocation)
	{
		//CreateFlippedBitmap();
		VoodooEngine::Engine->StoredUpdateComponents.push_back(this);
	}

	void OnGameObjectDeleted()
	{
		VoodooEngine::Engine->RemoveComponent(
			(UpdateComponent*)this, &VoodooEngine::Engine->StoredUpdateComponents);
		
		if (GameObjectFlippedBitmap.Bitmap)
		{
			VoodooEngine::Engine->RemoveComponent(
				&GameObjectFlippedBitmap, &VoodooEngine::Engine->StoredBitmapComponents);
		}
		else
		{
			ScreenPrint(VoodooEngine::Engine, "flipped_bitmap_not_valid_character_class");
		}

		GameObjectFlippedBitmap.Bitmap = nullptr;

		ScreenPrint(VoodooEngine::Engine, "deleted_character");
	}

	/*
	void SetGameObjectState(bool Enable)
	{
		//GameObject::SetGameObjectState(Enable);

		//ScreenPrint(VoodooEngine::Engine, "called_character_virtual_setgameobjectstate");

		if (Enable)
		{
			//GameObjectFlippedBitmap.BitmapParams.BitmapSetToNotRender = false;
		}
		else if (!Enable)
		{
			//GameObjectFlippedBitmap.BitmapParams.BitmapSetToNotRender = true;
		}
	}

	void FlipBitmapBasedOnMovement()
	{
		if (MoveComp.MovementDirection.X > 0)
		{
			GameObjectFlippedBitmap.BitmapParams.BitmapSetToNotRender = true;
			GameObjectBitmap.BitmapParams.BitmapSetToNotRender = false;
			return;
		}
		else if (MoveComp.MovementDirection.X < 0)
		{
			GameObjectFlippedBitmap.BitmapParams.BitmapSetToNotRender = false;
			GameObjectBitmap.BitmapParams.BitmapSetToNotRender = true;
		}
	}

	void Update(float DeltaTime)
	{
		//FlipBitmapBasedOnMovement();
	}
	*/
};
