#pragma once

#include "DEngineCollisionTags.h"
#include "DDefaultRenderLayers.h"
#include "VoodooEngine.h"

class Gizmo : public Object, public UpdateComponent, public IInput
{
public:
	BitmapComponent GizmoBitmap;
	CollisionComponent GizmoCollision;

	// Will send event whenever a game object is moved by the gizmo
	std::vector<IEventNoParameters*> MoveGameObjectEventListeners;

	GameObject* SelectedGameObject = nullptr;
	GameObject* CurrentClickedGameObject = nullptr;
	bool GameObjectMouseHover = false;
	bool GizmoMouseHover = false;
	bool CanDragGizmo = false;
	SVector MouseClickLocationOffset;
	float GizmoCollisionRectSize = 70;
	bool RenderGizmoCollisionRect = false;

	void SetupGizmoCollisionTag()
	{
		GizmoCollision.CollisionTag = TAG_LEVEL_EDITOR_GIZMO;
	};

	void SetupGizmoCollisionRect()
	{
		GizmoCollision.CollisionRect.X = GizmoCollisionRectSize;
		GizmoCollision.CollisionRect.Y = GizmoCollisionRectSize;
		GizmoCollision.RenderCollisionRect = RenderGizmoCollisionRect;
	};

	void InitGizmo(VoodooEngine* Engine)
	{
		StaticEngineReference = Engine;
		SEditorAssetPathList Asset;
		GizmoBitmap.Bitmap = SetupBitmap(GizmoBitmap.Bitmap, Asset.Gizmo, Engine->Renderer);
		SetupBitmapComponent(&GizmoBitmap, GizmoBitmap.Bitmap);
		RenderGizmoCollisionRect = Engine->DebugMode;
		SetupGizmoCollisionTag();
		SetupGizmoCollisionRect();
		Engine->StoredEditorUpdateComponents.push_back(this);
		Engine->StoredEditorBitmapComponents.push_back(&GizmoBitmap);
		Engine->StoredEditorCollisionComponents.push_back(&GizmoCollision);
		Engine->InterfaceObjects_Input.push_back(this);
	};

	void InitGizmoLocation(SVector NewLocation)
	{
		Location = NewLocation;
	};

	void SetGizmoState(bool Hidden)
	{
		if (Hidden)
		{
			GizmoBitmap.BitmapParams.BitmapSetToNotRender = true;
			GizmoCollision.NoCollision = true;
		}
		else
		{
			GizmoBitmap.BitmapParams.BitmapSetToNotRender = false;
			GizmoCollision.NoCollision = false;
		}
	};

	void SetMouseClickGizmoLocationOffset()
	{
		MouseClickLocationOffset.X = Location.X - StaticEngineReference->Mouse.Location.X;
		MouseClickLocationOffset.Y = Location.Y - StaticEngineReference->Mouse.Location.Y;
	};

	void UpdateMouseDragSnapLocationGizmo()
	{
		if (CanDragGizmo &&
			StaticEngineReference->Mouse.PrimaryMousePressed)
		{
			Location.X =
				(int)((StaticEngineReference->Mouse.Location.X +
					MouseClickLocationOffset.X) / StaticEngineReference->LevelEditorGizmoSnapSize) *
				StaticEngineReference->LevelEditorGizmoSnapSize;
			Location.Y =
				(int)((StaticEngineReference->Mouse.Location.Y +
					MouseClickLocationOffset.Y) / StaticEngineReference->LevelEditorGizmoSnapSize) *
				StaticEngineReference->LevelEditorGizmoSnapSize;
		}
	};

	void UpdateGizmoLocation()
	{
		GizmoBitmap.ComponentLocation = Location;

		// Offset the x value to the left a bit since the collision is wider tham the bitmap
		GizmoCollision.ComponentLocation.X = Location.X - 5;

		GizmoCollision.ComponentLocation.Y = Location.Y;
	};

	void UpdateSelectedGameObjectDragLocation()
	{
		if (CanDragGizmo &&
			SelectedGameObject != nullptr &&
			StaticEngineReference->Mouse.PrimaryMousePressed)
		{
			SVector NewLocation =
			{ Location.X - GetGizmoOffsetLocation().X,
			Location.Y - GetGizmoOffsetLocation().Y };

			SelectedGameObject->Location = NewLocation;
			SelectedGameObject->GameObjectBitmap.ComponentLocation = NewLocation;
			SelectedGameObject->DefaultGameObjectCollision.ComponentLocation = NewLocation;

			for (int i = 0; i < MoveGameObjectEventListeners.size(); ++i)
			{
				MoveGameObjectEventListeners[i]->InterfaceEvent_NoParams();
			}
		}
	};

	bool IsMouseHoveringGizmo()
	{
		if (IsCollisionDetected(&GizmoCollision, &StaticEngineReference->Mouse.MouseCollider))
		{
			return true;
		}

		return false;
	};

	bool IsMouseHoveringGameObject()
	{
		bool CollisionDetected = false;
		for (int GameObjectIndex = 0; GameObjectIndex < StaticEngineReference->StoredGameObjects.size(); ++GameObjectIndex)
		{
			if (IsCollisionDetected(&StaticEngineReference->Mouse.MouseCollider,
				&StaticEngineReference->StoredGameObjects[GameObjectIndex]->DefaultGameObjectCollision))
			{
				CollisionDetected = true;
			}
		}

		return CollisionDetected;
	};

	int SetCurrentRenderLayerPrioritization(std::vector<GameObject*> CurrentGameObjectsFound)
	{
		std::vector<int> StoredRenderLayerIDs;
		for (int i = 0; i < CurrentGameObjectsFound.size(); ++i)
		{
			switch (CurrentGameObjectsFound[i]->GameObjectBitmap.BitmapParams.RenderLayer)
			{
			case RENDERLAYER_0:
				StoredRenderLayerIDs.push_back(RENDERLAYER_0);
				break;
			case RENDERLAYER_1:
				StoredRenderLayerIDs.push_back(RENDERLAYER_1);
				break;
			case RENDERLAYER_2:
				StoredRenderLayerIDs.push_back(RENDERLAYER_2);
				break;
			case RENDERLAYER_3:
				StoredRenderLayerIDs.push_back(RENDERLAYER_3);
				break;
			case RENDERLAYER_4:
				StoredRenderLayerIDs.push_back(RENDERLAYER_4);
				break;
			case RENDERLAYER_5:
				StoredRenderLayerIDs.push_back(RENDERLAYER_5);
				break;
			case RENDERLAYER_6:
				StoredRenderLayerIDs.push_back(RENDERLAYER_6);
				break;
			case RENDERLAYER_7:
				StoredRenderLayerIDs.push_back(RENDERLAYER_7);
				break;
			case RENDERLAYER_8:
				StoredRenderLayerIDs.push_back(RENDERLAYER_8);
				break;
			case RENDERLAYER_9:
				StoredRenderLayerIDs.push_back(RENDERLAYER_9);
				break;
			case RENDERLAYER_10:
				StoredRenderLayerIDs.push_back(RENDERLAYER_10);
				break;
			}
		}

		int CurrentRenderLayerPrio = 0;
		for (int IDIndex = 0; IDIndex < StoredRenderLayerIDs.size(); ++IDIndex)
		{
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_0)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_1)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_2)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_3)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_4)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_5)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_6)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_7)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_8)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_9)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
			if (StoredRenderLayerIDs[IDIndex] > RENDERLAYER_10)
			{
				CurrentRenderLayerPrio = StoredRenderLayerIDs[IDIndex];
			}
		}

		return CurrentRenderLayerPrio;
	};

	GameObject* AssignSelectedGameObjectByRenderLayer(
		std::vector<GameObject*> GameObjectsFound, int RenderLayerPrioritized)
	{
		int GameObjectToAssign = 0;
		for (int GameObjectIndex = 0; GameObjectIndex < GameObjectsFound.size(); ++GameObjectIndex)
		{
			// If multiple game objects are colliding, then pick the one with the highest render layer prio
			if (GameObjectsFound[GameObjectIndex]->GameObjectBitmap.BitmapParams.RenderLayer ==
				RenderLayerPrioritized)
			{
				GameObjectToAssign = GameObjectIndex;
			}
		}

		return GameObjectsFound[GameObjectToAssign];
	}

	void AssignMouseClickedGameObject()
	{
		StaticEngineReference->Mouse.MouseHoveredObject = nullptr;
		std::vector<GameObject*> GameObjectsFound;
		for (int GameObjectIndex = 0; GameObjectIndex < StaticEngineReference->StoredGameObjects.size(); ++GameObjectIndex)
		{
			if (IsCollisionDetected(&StaticEngineReference->Mouse.MouseCollider,
				&StaticEngineReference->StoredGameObjects[GameObjectIndex]->DefaultGameObjectCollision))
			{
				GameObjectsFound.push_back(StaticEngineReference->StoredGameObjects[GameObjectIndex]);
			}
		}

		if (GameObjectsFound.size() > 1)
		{
			StaticEngineReference->Mouse.MouseHoveredObject = AssignSelectedGameObjectByRenderLayer(
				GameObjectsFound, SetCurrentRenderLayerPrioritization(GameObjectsFound));
		}
		else if (!GameObjectsFound.empty())
		{
			StaticEngineReference->Mouse.MouseHoveredObject = GameObjectsFound.at(0);
		}
	}

	void Update(float DeltaTime)
	{
		if (StaticEngineReference->GameRunning)
		{
			return;
		}

		// Enables continous check for collision with game objects, 
		// instead of once per overlap/end overlap
		StaticEngineReference->Mouse.MouseCollider.IsOverlapped = false;

		GizmoMouseHover = IsMouseHoveringGizmo();
		GameObjectMouseHover = IsMouseHoveringGameObject();
		UpdateMouseDragSnapLocationGizmo();
		UpdateGizmoLocation();
		UpdateSelectedGameObjectDragLocation();
	};

	SVector GetGizmoOffsetLocation()
	{
		SVector OffsetLocation =
		{ (SelectedGameObject->DefaultGameObjectCollision.CollisionRect.X / 2) - 10,
			(SelectedGameObject->DefaultGameObjectCollision.CollisionRect.Y / 2) -
			GizmoBitmap.BitmapParams.BitmapSource.Y + 10 };

		return OffsetLocation;
	};

	void SetGizmoLocationToSelectedGameObject()
	{
		if (SelectedGameObject != nullptr)
		{
			SVector CenterLocation;
			CenterLocation.X = SelectedGameObject->Location.X + GetGizmoOffsetLocation().X;
			CenterLocation.Y = SelectedGameObject->Location.Y + GetGizmoOffsetLocation().Y;

			Location = CenterLocation;
		}
	};

	void AssignSelectedObject()
	{
		if (StaticEngineReference->Mouse.MouseHoveredObject != nullptr)
		{
			SelectedGameObject = (GameObject*)(StaticEngineReference->Mouse.MouseHoveredObject);
		}
	};

	void FullGizmoReset()
	{
		CanDragGizmo = false;
		SetGizmoState(true);
		StaticEngineReference->Mouse.MouseHoveredObject = nullptr;
		SelectedGameObject = nullptr;
	};

	void InterfaceEvent_Input(int Input, bool Pressed)
	{
		if (StaticEngineReference->GameRunning)
		{
			FullGizmoReset();
			return;
		}

		if (StaticEngineReference->Mouse.PrimaryMousePressed)
		{
			SetMouseClickGizmoLocationOffset();

			CanDragGizmo = false;
			if (!GameObjectMouseHover &&
				!GizmoMouseHover)
			{
				FullGizmoReset();
			}
			else if (GizmoMouseHover)
			{
				CanDragGizmo = true;
			}

			AssignMouseClickedGameObject();

			CurrentClickedGameObject = nullptr;
			if (GameObjectMouseHover &&
				StaticEngineReference->Mouse.MouseHoveredObject != nullptr)
			{
				CurrentClickedGameObject = (GameObject*)(StaticEngineReference->Mouse.MouseHoveredObject);
			}

			if (!GizmoMouseHover &&
				GameObjectMouseHover &&
				CurrentClickedGameObject != SelectedGameObject)
			{
				SetGizmoState(false);
				AssignSelectedObject();
				SetGizmoLocationToSelectedGameObject();
			}
		}
	};

private:
	VoodooEngine* StaticEngineReference = nullptr;
};
