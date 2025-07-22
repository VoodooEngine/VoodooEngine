/*
#pragma once

#include "DLevelEditorInfo.h"
#include "VoodooEngine.h"
#include <commdlg.h>
#include "Gizmo.h"

// Button locations
#define BUTTON_LOC_X_OPENLEVEL 20
#define BUTTON_LOC_Y_OPENLEVEL 20
#define BUTTON_LOC_X_SAVELEVEL 180
#define BUTTON_LOC_Y_SAVELEVEL 20
#define BUTTON_LOC_X_PLAYLEVEL 890
#define BUTTON_LOC_Y_PLAYLEVEL 20
#define BUTTON_LOC_X_PREVIOUS 1615
#define BUTTON_LOC_X_NEXT 1770
#define BUTTON_LOC_Y_NEXT_PREVIOUS 100
#define BUTTON_LOC_X_ASSETBROWSER 1740
#define BUTTON_LOC_Y_ASSETBROWSER 20
#define BUTTON_LOC_X_RENDERLAYER 1560
#define BUTTON_LOC_Y_RENDERLAYER 20
#define BUTTON_LOC_X_VIEWMODE 1400
#define BUTTON_LOC_Y_VIEWMODE 20

// Asset selection grid location values
#define ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_1 0
#define ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_2 100
#define ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_3 200
#define ASSET_SELECTION_GRID_OFFSETLOC_ROW_1 0
#define ASSET_SELECTION_BUTTON_LOC_X_ORIGIN 1615
#define ASSET_SELECTION_BUTTON_LOC_Y_ORIGIN 165

// Max number of assets allowed to be displayed for each column (in asset selection grid)
#define ASSET_SELECTION_GRID_MAXNUM_COLUMN_1 9
#define ASSET_SELECTION_GRID_MAXNUM_COLUMN_2 18
#define ASSET_SELECTION_GRID_MAXNUM_COLUMN_3 27
// Total number of assets allowed to be displayed at any time 
#define ASSET_SELECTION_GRID_MAXNUM_DISPLAYED 27

// Asset location spawned when asset button is clicked
#define ASSET_SELECTION_SPAWN_LOCATION_X 850
#define ASSET_SELECTION_SPAWN_LOCATION_Y 400

class VoodooLevelEditor : public Object, public UpdateComponent, public IInput, public IEventNoParameters
{

enum EMenuType
{
	None,
	AssetBrowser,
	RenderLayer,
	ViewMode
};

public:
	VoodooLevelEditor()
	{
		// Add level editor update function to be called every frame
		VoodooEngine::Engine->StoredEditorUpdateComponents.push_back(this);

		// Add input callback for level editor
		VoodooEngine::Engine->InterfaceObjects_Input.push_back(this);

		// Create level editor UI
		LevelEditorUITop.Bitmap =
			SetupBitmap(LevelEditorUITop.Bitmap, Asset.LevelEditorUITop, VoodooEngine::Engine->Renderer);
		SetupBitmapComponent(&LevelEditorUITop, LevelEditorUITop.Bitmap);

		LevelEditorUIOverlay.Bitmap =
			SetupBitmap(LevelEditorUIOverlay.Bitmap, Asset.LevelEditorUIOverlay, VoodooEngine::Engine->Renderer);
		SetupBitmapComponent(&LevelEditorUIOverlay, LevelEditorUIOverlay.Bitmap);

		VoodooEngine::Engine->StoredEditorBitmapComponents.push_back(&LevelEditorUIOverlay);
		VoodooEngine::Engine->StoredEditorBitmapComponents.push_back(&LevelEditorUITop);

		// Create all render layer visibility eye icons 
		// (used to pick which render layer show/hide
		CreateRenderLayerVisiblityEyeIconButtons();
		// Create all clickable buttons
		CreateAllLevelEditorButtons();
		CreateAssetButtons();
		// Display current asset section 
		DisplayAssets(0);

		LevelEditorVisible = true;
		SetMenuVisible(EMenuType::ViewMode);

		CreateGizmo();
	}

	inline static VoodooLevelEditor* LevelEditor = nullptr;

	Gizmo TransformGizmo;
	void(*ButtonPressedCallback)(int);
	bool LevelEditorVisible = false;
	bool AssetBrowserVisible = false;
	EMenuType CurrentMenuTypeActivated = EMenuType::None;

	// This is called whenever a game object is moved by the gizmo
	void InterfaceEvent_NoParams()
	{
		if (LevelEditorVisible)
		{
			SaveStateChanged(false);
		}
	};
	void CreateGizmo()
	{
		TransformGizmo.InitGizmo(VoodooEngine::Engine);
		TransformGizmo.InitGizmoLocation({ 1000, 500 });
		TransformGizmo.SetGizmoState(true);

		TransformGizmo.MoveGameObjectEventListeners.push_back(this);
	};
	void CreateAssetButtons()
	{
		// First get all stored game object and store them in "CurrentStoredButtonAssets"
		AddAssetContentForLevelEditor();

		float LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_1;
		float LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
		float OffsetYAmount = 100;

		int Index = 0;
		for (int i = 0; i < CurrentStoredButtonAssets.size(); ++i)
		{
			int CurrentAssetID = CurrentStoredButtonAssets[i].AssetID;

			// Move to next column if at max amount of assets displayed at column 1
			if (Index == ASSET_SELECTION_GRID_MAXNUM_COLUMN_1)
			{
				LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_2;
				LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
			}

			// Move to next column if at max amount of assets displayed at column 2
			if (Index == ASSET_SELECTION_GRID_MAXNUM_COLUMN_2)
			{
				LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_3;
				LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
			}

			// Reset to first column if at max amount of assets displayed at column 3
			if (Index == ASSET_SELECTION_GRID_MAXNUM_COLUMN_3)
			{
				LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_1;
				LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;

				Index = 0;
			}

			Button* AssetButton = nullptr;
			AssetButton = CreateButton(
				VoodooEngine::Engine, AssetButton, CurrentAssetID, AssetButtonThumbnail, "",
				{ ASSET_SELECTION_BUTTON_LOC_X_ORIGIN + LocXOffset,
				(ASSET_SELECTION_BUTTON_LOC_Y_ORIGIN + LocYOffset) },
				CurrentStoredButtonAssets[i].AssetParams.AssetFilePath.c_str(),
				CurrentStoredButtonAssets[i].AssetParams.AssetButtonThumbnailTextureAtlasHeight,
				CurrentStoredButtonAssets[i].AssetParams.AssetButtonThumbnailTextureAtlasOffsetMultiplierY);

			CurrentStoredButtonAssets[i].AssetButton = AssetButton;
			LocYOffset += OffsetYAmount;

			Index++;
		}
	};
	void CreateRenderLayerVisiblityEyeIconButtons()
	{
		SEditorAssetPathList AssetList;
		SVector OriginLocation = { 1620, 110 };
		float OffsetLocationY = 50;
		for (int i = 0; i < VoodooEngine::Engine->StoredLevelEditorRenderLayers.size(); ++i)
		{
			auto Iterator = VoodooEngine::Engine->StoredLevelEditorRenderLayers.find(i);
			Button* EyeIcon = nullptr;
			EyeIcon = CreateButton(VoodooEngine::Engine, EyeIcon, Iterator->first,
				EButtonType::TwoSided, "", OriginLocation, AssetList.RenderLayerEyeIcon);
			RenderLayerVisibilityEyeIconButtons.push_back(EyeIcon);

			OriginLocation.Y += OffsetLocationY;
		}
	};
	void SetPlayLevelButtonState(EButtonState NewState)
	{
		HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;

		if (NewState == EButtonState::Default)
		{
			SetButtonState(PlayLevelButton, EButtonState::Default);
		}
		else
		{
			SetButtonState(PlayLevelButton, EButtonState::Hidden);
		}
	};
	void SetStopPlayButtonState(EButtonState NewState)
	{
		HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;

		if (NewState == EButtonState::Default)
		{
			SetButtonState(StopPlayButton, EButtonState::Default, true);
		}
		else
		{
			SetButtonState(StopPlayButton, EButtonState::Hidden, true);
		}
	};
	void DeleteAssetButtons()
	{
		// Remove assets from vector
		// delete both components created with "new": bitmapcomponent, collisioncomponent
		// And finally delete button created with "new"
		for (int i = 0; i < CurrentStoredButtonAssets.size(); ++i)
		{
			while (!CurrentStoredButtonAssets.empty())
			{
				if (!CurrentStoredButtonAssets[i].AssetButton)
				{
					return;
				}

				SAssetButton AssetToDelete = CurrentStoredButtonAssets[i];

				CurrentStoredButtonAssets.erase(std::remove(CurrentStoredButtonAssets.begin(),
					CurrentStoredButtonAssets.end(), AssetToDelete));

				VoodooEngine::Engine->StoredButtonBitmapComponents.erase(std::remove(
					VoodooEngine::Engine->StoredButtonBitmapComponents.begin(),
					VoodooEngine::Engine->StoredButtonBitmapComponents.end(), 
					&AssetToDelete.AssetButton->ButtonBitmap));

				VoodooEngine::Engine->StoredEditorCollisionComponents.erase(std::remove(
					VoodooEngine::Engine->StoredEditorCollisionComponents.begin(),
					VoodooEngine::Engine->StoredEditorCollisionComponents.end(), 
					&AssetToDelete.AssetButton->ButtonCollider));

				delete AssetToDelete.AssetButton;
			}
		}

		std::vector<SAssetButton>().swap(CurrentStoredButtonAssets);
	};
	void ResetButtonsBitmapSource(Button* ButtonToSet)
	{
		if (!ButtonToSet)
		{
			return;
		}

		SetButtonState(ButtonToSet, EButtonState::Default);
	};
	void SetButtonBitmapSourceClicked(Button* ButtonToSet)
	{
		if (!ButtonToSet)
		{
			return;
		}

		SetButtonState(ButtonToSet, EButtonState::Disabled);
	};
	void UpdateAssetThumbnailButtonsState(EButtonState NewButtonState)
	{
		for (int i = 0; i < CurrentStoredButtonAssets.size(); ++i)
		{
			SetButtonState(CurrentStoredButtonAssets[i].AssetButton, NewButtonState, true);
		}
	};
	void UpdateAllButtonsState(EButtonState NewButtonState)
	{
		UpdateAssetThumbnailButtonsState(NewButtonState);
		SetButtonState(OpenLevelButton, NewButtonState, true);
		SetButtonState(PlayLevelButton, NewButtonState, true);
		SetButtonState(StopPlayButton, NewButtonState, true);
		if (NewButtonState == EButtonState::Hidden)
		{
			SetMenuVisible(EMenuType::None);
			SetButtonState(SaveLevelButton, EButtonState::Hidden);
		}
		else
		{
			SetMenuVisible(MenuSelectedBeforeHidden);
			if (ChangesMadeSinceLastSave)
			{
				SetButtonState(SaveLevelButton, EButtonState::Default);
			}
			else
			{
				SetButtonState(SaveLevelButton, EButtonState::Disabled);
			}
		}
	};
	void UpdateLevelEditorVisibility(bool Hide)
	{
		if (Hide)
		{
			if (!VoodooEngine::Engine->GameRunning)
			{
				MenuSelectedBeforeHidden = CurrentMenuTypeActivated;
			}

			HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
			LevelEditorVisible = false;
			LevelEditorUITop.BitmapParams.BitmapSetToNotRender = true;
			LevelEditorUIOverlay.BitmapParams.BitmapSetToNotRender = true;
			UpdateAllButtonsState(EButtonState::Hidden);
			VoodooEngine::Engine->SetMouseState(VoodooEngine::Engine, false);
		}
		else if (!Hide)
		{
			VoodooEngine::Engine->SetMouseState(VoodooEngine::Engine, true);
			LevelEditorVisible = true;
			LevelEditorUITop.BitmapParams.BitmapSetToNotRender = false;
			if (!VoodooEngine::Engine->GameRunning)
			{
				UpdateAllButtonsState(EButtonState::Default);
				SetButtonState(StopPlayButton, EButtonState::Hidden, true);
			}
			else
			{
				SetButtonState(StopPlayButton, EButtonState::Default, true);
			}
		}
	};
	void SetAllRenderLayerUITextVisibility(bool ShowText)
	{
		bool HideRenderLayerUIText = false;
		if (!ShowText)
		{
			HideRenderLayerUIText = true;
		}

		for (int i = 0; i < VoodooEngine::Engine->StoredLevelEditorRenderLayers.size(); ++i)
		{
			auto Iterator = VoodooEngine::Engine->StoredLevelEditorRenderLayers.find(i);
			Iterator->second.HideText = HideRenderLayerUIText;
		}
	};
	void SetAllRenderLayerEyeIconButtonsState(EButtonState NewButtonState)
	{
		bool SetToHide = true;
		bool DisableCollision = true;
		switch (NewButtonState)
		{
		case Default:
			SetToHide = false;
			DisableCollision = false;
			break;
		// Never gets here (disabled case) 
		// since eye icon buttons are either shown or hidden but never disabled
		case Disabled:
			break;
		case Hidden:
			SetToHide = true;
			DisableCollision = true;
			break;
		}

		for (int i = 0; i < RenderLayerVisibilityEyeIconButtons.size(); ++i)
		{
			RenderLayerVisibilityEyeIconButtons[i]->ButtonBitmap.BitmapParams.BitmapSetToNotRender = SetToHide;
			RenderLayerVisibilityEyeIconButtons[i]->ButtonCollider.NoCollision = DisableCollision;
		}
	};
	void SetGameObjectsVisibilityBasedOnRenderLayer(bool EnableRenderLayer, int RenderLayer)
	{
		for (int i = 0; i < VoodooEngine::Engine->StoredGameObjects.size(); ++i)
		{
			if (VoodooEngine::Engine->StoredGameObjects[i]->GameObjectBitmap.BitmapParams.RenderLayer ==
				RenderLayer)
			{
				if (EnableRenderLayer)
				{
					VoodooEngine::Engine->StoredGameObjects[i]->
						GameObjectBitmap.BitmapParams.BitmapSetToNotRender = false;
					VoodooEngine::Engine->StoredGameObjects[i]->
						DefaultGameObjectCollision.NoCollision = false;
				}
				else
				{
					VoodooEngine::Engine->StoredGameObjects[i]->
						GameObjectBitmap.BitmapParams.BitmapSetToNotRender = true;
					VoodooEngine::Engine->StoredGameObjects[i]->
						DefaultGameObjectCollision.NoCollision = true;
				}
			}
		}
	};
	void SetRenderLayerEyeIconButtonState()
	{
		// Do nothing if ID is out of range of render layers
		if (HoveredButtonID > RenderLayerVisibilityEyeIconButtons.size())
		{
			return;
		}

		int BitmapWidth =
			RenderLayerVisibilityEyeIconButtons.at(
				HoveredButtonID)->ButtonBitmap.Bitmap->GetSize().width / 2;
		auto Iterator = VoodooEngine::Engine->StoredLevelEditorRenderLayers.find(HoveredButtonID);
		if (Iterator->second.TextRenderType == ETextBrushColorType::BlackBrush)
		{
			// Show "on" eye icon bitmap
			SetBitmapSourceLocationX(
				&RenderLayerVisibilityEyeIconButtons.at(
					HoveredButtonID)->ButtonBitmap, BitmapWidth, 1);

			Iterator->second.TextRenderType = ETextBrushColorType::WhiteBrush;
			SetGameObjectsVisibilityBasedOnRenderLayer(true, HoveredButtonID);
		}
		else if (Iterator->second.TextRenderType == ETextBrushColorType::WhiteBrush)
		{
			// Show "off" eye icon bitmap
			SetBitmapSourceLocationX(
				&RenderLayerVisibilityEyeIconButtons.at(
					HoveredButtonID)->ButtonBitmap, BitmapWidth, 2);

			Iterator->second.TextRenderType = ETextBrushColorType::BlackBrush;
			SetGameObjectsVisibilityBasedOnRenderLayer(false, HoveredButtonID);
		}
	};
	void SetMenuVisible(EMenuType MenuToShow)
	{
		switch (MenuToShow)
		{
		case VoodooLevelEditor::None:
			AssetBrowserVisible = false;
			UpdateAssetThumbnailButtonsState(EButtonState::Hidden);
			SetButtonState(NextButton, EButtonState::Hidden);
			SetButtonState(PreviousButton, EButtonState::Hidden);
			SetButtonState(AssetBrowserButton, EButtonState::Hidden);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Hidden);
			SetButtonState(ViewModeSelectionButton, EButtonState::Hidden);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Hidden);
			SetAllRenderLayerUITextVisibility(false);
			LevelEditorUIOverlay.BitmapParams.BitmapSetToNotRender = true;
			break;
		case VoodooLevelEditor::AssetBrowser:
			AssetBrowserVisible = true;
			UpdateAssetThumbnailButtonsState(EButtonState::Default);
			SetButtonState(NextButton, EButtonState::Default);
			SetButtonState(PreviousButton, EButtonState::Default);
			SetButtonState(AssetBrowserButton, EButtonState::Disabled);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Default);
			SetButtonState(ViewModeSelectionButton, EButtonState::Default);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Hidden);
			SetAllRenderLayerUITextVisibility(false);
			if (!VoodooEngine::Engine->GameRunning)
			{
				LevelEditorUIOverlay.BitmapParams.BitmapSetToNotRender = false;
			}
			break;
		case VoodooLevelEditor::RenderLayer:
			AssetBrowserVisible = false;
			UpdateAssetThumbnailButtonsState(EButtonState::Hidden);
			SetButtonState(NextButton, EButtonState::Hidden);
			SetButtonState(PreviousButton, EButtonState::Hidden);
			SetButtonState(AssetBrowserButton, EButtonState::Default);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Disabled);
			SetButtonState(ViewModeSelectionButton, EButtonState::Default);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Default);
			SetAllRenderLayerUITextVisibility(true);
			if (!VoodooEngine::Engine->GameRunning)
			{
				LevelEditorUIOverlay.BitmapParams.BitmapSetToNotRender = false;
			}
			break;
		case VoodooLevelEditor::ViewMode:
			AssetBrowserVisible = false;
			UpdateAssetThumbnailButtonsState(EButtonState::Hidden);
			SetButtonState(NextButton, EButtonState::Hidden);
			SetButtonState(PreviousButton, EButtonState::Hidden);
			SetButtonState(AssetBrowserButton, EButtonState::Default);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Default);
			SetButtonState(ViewModeSelectionButton, EButtonState::Disabled);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Hidden);
			SetAllRenderLayerUITextVisibility(false);
			LevelEditorUIOverlay.BitmapParams.BitmapSetToNotRender = true;
			break;
		}

		CurrentMenuTypeActivated = MenuToShow;
	};
	void OnButtonPressed()
	{
		if (ButtonPressedCallback)
		{
			ButtonPressedCallback(HoveredButtonID);
		}

		switch (HoveredButtonID)
		{
		case TAG_LEVEL_EDITOR_BUTTON_SAVELEVEL:
			VoodooEngine::Engine->SaveLevelFile();
			SetButtonBitmapSourceClicked(SaveLevelButton);
			SaveStateChanged(true);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL:
			OpenLevelFile(VoodooEngine::Engine);
			SaveStateChanged(false);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL:
			VoodooEngine::Engine->StartGame();
			TransformGizmo.SetGizmoState(true);
			MenuSelectedBeforeHidden = CurrentMenuTypeActivated;
			UpdateAllButtonsState(EButtonState::Hidden);
			SetStopPlayButtonState(EButtonState::Default);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_STOPPLAY:
			VoodooEngine::Engine->EndGame();
			TransformGizmo.FullGizmoReset();
			UpdateAllButtonsState(EButtonState::Default);
			SetStopPlayButtonState(EButtonState::Hidden);
			SetMenuVisible(MenuSelectedBeforeHidden);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS:
			SetButtonBitmapSourceClicked(PreviousButton);
			DisplayAssets(TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT:
			SetButtonBitmapSourceClicked(NextButton);
			DisplayAssets(TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_ASSETS:
			SetMenuVisible(EMenuType::AssetBrowser);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_RENDERLAYERS:
			SetMenuVisible(EMenuType::RenderLayer);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_VIEWMODE:
			SetMenuVisible(EMenuType::ViewMode);
			break;
		default:
			// If no editor button is the button ID that was hovered, 
			// then this switch statement gets executed 
			switch (CurrentMenuTypeActivated)
			{
				// Do nothing
			case VoodooLevelEditor::None:
				break;
				// In asset browser mode, 
				// when asset button is clicked a game object based on ID is spawned
			case VoodooLevelEditor::AssetBrowser:
				if (VoodooEngine::Engine->FunctionPointer_LoadGameObjects)
				{
					// When a game object is selected to spawn from asset menu,
					// pass an empty vector since it is only used for storing gameobjects to levels, 
					// when a level is loaded
					std::vector<GameObject*> EmptyVector;
					VoodooEngine::Engine->FunctionPointer_LoadGameObjects(
						HoveredButtonID,
						{ ASSET_SELECTION_SPAWN_LOCATION_X, ASSET_SELECTION_SPAWN_LOCATION_Y },
						EmptyVector);
					SaveStateChanged(false);
				}
				break;
				// In render layer selection mode,
				// when eye icon button is clicked, 
				// the game object assosiated with the eye icon's render layer,
				// will be toggled visible/hidden
			case VoodooLevelEditor::RenderLayer:
				// We use toggle (on/off) for each eye icon button
				// the current state is determined internally for each eye icon button when clicked
				// so if button is clicked when set to "on" it will be set to "off" and vice versa
				SetRenderLayerEyeIconButtonState();
				break;
			}
			break;
		}
	};
	void InterfaceEvent_Input(int Input, bool Pressed)
	{
		if (Input == VK_TAB)
		{
			if (Pressed)
			{
				if (LevelEditorVisible)
				{
					UpdateLevelEditorVisibility(true);
				}
				else if (!LevelEditorVisible)
				{
					UpdateLevelEditorVisibility(false);
				}
			}
		}

		// Delete selected game object
		if (VoodooEngine::Engine->GameRunning == false &&
			Input == VK_DELETE &&
			TransformGizmo.SelectedGameObject != nullptr)
		{
			VoodooEngine::Engine->DeleteGameObject(TransformGizmo.SelectedGameObject);
			TransformGizmo.SelectedGameObject = nullptr;
			TransformGizmo.SetGizmoState(true);
			SaveStateChanged(false);
		}

		if (!LevelEditorVisible)
		{
			return;
		}

		if (VoodooEngine::Engine->Mouse.PrimaryMousePressed)
		{
			OnButtonPressed();
		}
		else if (!VoodooEngine::Engine->Mouse.PrimaryMousePressed)
		{
			if (AssetBrowserVisible)
			{
				ResetButtonsBitmapSource(PreviousButton);
				ResetButtonsBitmapSource(NextButton);
			}
		}
	};
	void UpdateButtonCollisionCheck(Button* ButtonToUpdate)
	{
		if (ButtonToUpdate == nullptr)
		{
			return;
		}

		BroadcastCollision(this, &ButtonToUpdate->ButtonCollider, &VoodooEngine::Engine->Mouse.MouseCollider);
	};
	void UpdateRenderLayerEyeIconButtonsCollisionCheck()
	{
		for (int i = 0; i < RenderLayerVisibilityEyeIconButtons.size(); ++i)
		{
			UpdateButtonCollisionCheck(RenderLayerVisibilityEyeIconButtons[i]);
		}
	};
	void Update(float DeltaTime)
	{
		UpdateButtonCollisionCheck(OpenLevelButton);
		UpdateButtonCollisionCheck(SaveLevelButton);
		UpdateButtonCollisionCheck(PlayLevelButton);
		UpdateButtonCollisionCheck(StopPlayButton);
		UpdateButtonCollisionCheck(PreviousButton);
		UpdateButtonCollisionCheck(NextButton);
		UpdateButtonCollisionCheck(AssetBrowserButton);
		UpdateButtonCollisionCheck(RenderLayerSelectionButton);
		UpdateButtonCollisionCheck(ViewModeSelectionButton);
		UpdateRenderLayerEyeIconButtonsCollisionCheck();

		for (int i = 0; i < CurrentStoredButtonAssets.size(); ++i)
		{
			if (!CurrentStoredButtonAssets[i].AssetButton)
			{
				continue;
			}

			BroadcastCollision(
				this, &CurrentStoredButtonAssets[i].AssetButton->ButtonCollider,
				&VoodooEngine::Engine->Mouse.MouseCollider);
		}
	};
	void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag, Object* Target)
	{
		HoveredButtonID = SenderCollisionTag;
	};
	void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag)
	{
		if (SenderCollisionTag == HoveredButtonID)
		{
			HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
		}
	};

private:
	struct SAssetIndex
	{
		int Min = 0;
		int Max = ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
	};

	int HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
	SEditorAssetPathList Asset;
	SAssetIndex AssetIndexDisplayed;
	std::vector<SAssetButton> CurrentStoredButtonAssets;
	bool ChangesMadeSinceLastSave = false;
	EMenuType MenuSelectedBeforeHidden = EMenuType::ViewMode;
	BitmapComponent LevelEditorUITop;
	BitmapComponent LevelEditorUIOverlay;
	Button* OpenLevelButton = nullptr;
	Button* SaveLevelButton = nullptr;
	Button* PlayLevelButton = nullptr;
	Button* StopPlayButton = nullptr;
	Button* PreviousButton = nullptr;
	Button* NextButton = nullptr;
	Button* AssetBrowserButton = nullptr;
	Button* RenderLayerSelectionButton = nullptr;
	Button* ViewModeSelectionButton = nullptr;
	std::vector<Button*> RenderLayerVisibilityEyeIconButtons;

	void CreateAllLevelEditorButtons()
	{
		PlayLevelButton = CreateButton(VoodooEngine::Engine, PlayLevelButton, TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL,
			TwoSided, "play_level", { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonW140);
		StopPlayButton = CreateButton(
			VoodooEngine::Engine, StopPlayButton, TAG_LEVEL_EDITOR_BUTTON_STOPPLAY,
			TwoSided, "stop_play", { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonActivateDeactivateW140);
		SetBitmapSourceLocationX(
			&StopPlayButton->ButtonBitmap,
			StopPlayButton->ButtonBitmap.Bitmap->GetSize().width / 2, 2);
		SetStopPlayButtonState(EButtonState::Hidden);
		OpenLevelButton = CreateButton(
			VoodooEngine::Engine, OpenLevelButton, TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL,
			TwoSided, "open_level", { BUTTON_LOC_X_OPENLEVEL, BUTTON_LOC_Y_OPENLEVEL },
			Asset.LevelEditorButtonW140);
		SaveLevelButton = CreateButton(
			VoodooEngine::Engine, SaveLevelButton, TAG_LEVEL_EDITOR_BUTTON_SAVELEVEL,
			TwoSided, "save_level", { BUTTON_LOC_X_SAVELEVEL, BUTTON_LOC_Y_SAVELEVEL },
			Asset.LevelEditorButtonW140);
		PreviousButton = CreateButton(
			VoodooEngine::Engine, PreviousButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS,
			TwoSided, "previous", { BUTTON_LOC_X_PREVIOUS, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonW140);
		NextButton = CreateButton(
			VoodooEngine::Engine, NextButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT,
			TwoSided, "next", { BUTTON_LOC_X_NEXT, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonW140);
		AssetBrowserButton = CreateButton(
			VoodooEngine::Engine, AssetBrowserButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_ASSETS,
			TwoSided, "assetbrowser", { BUTTON_LOC_X_ASSETBROWSER, BUTTON_LOC_Y_ASSETBROWSER },
			Asset.LevelEditorButtonW160);
		RenderLayerSelectionButton = CreateButton(
			VoodooEngine::Engine, RenderLayerSelectionButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_RENDERLAYERS,
			TwoSided, "renderlayer", { BUTTON_LOC_X_RENDERLAYER, BUTTON_LOC_Y_RENDERLAYER },
			Asset.LevelEditorButtonW160);
		ViewModeSelectionButton = CreateButton(
			VoodooEngine::Engine, ViewModeSelectionButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_VIEWMODE,
			TwoSided, "viewmode", { BUTTON_LOC_X_VIEWMODE, BUTTON_LOC_Y_VIEWMODE },
			Asset.LevelEditorButtonW140);
	}
	void SaveStateChanged(bool Saved)
	{
		if (Saved)
		{
			ChangesMadeSinceLastSave = false;
			SetButtonState(SaveLevelButton, EButtonState::Disabled);
		}
		else
		{
			ChangesMadeSinceLastSave = true;
			if (LevelEditorVisible)
			{
				SetButtonState(SaveLevelButton, EButtonState::Default);
			}
		}
	};
	void AddLevelEditorAssetButton(
		int AssetID, bool CreateAssetCollision,
		std::wstring AssetPath,
		float AssetButtonThumbnailTextureAtlasHeight,
		float AssetButtonThumbnailTextureAltasOffsetMultiplierY)
	{
		SAssetButton AssetButton;
		AssetButton.AssetID = AssetID;
		AssetButton.AssetParams.AssetFilePath = AssetPath;
		AssetButton.AssetParams.AssetButtonThumbnailTextureAtlasHeight =
			AssetButtonThumbnailTextureAtlasHeight;
		AssetButton.AssetParams.AssetButtonThumbnailTextureAtlasOffsetMultiplierY =
			AssetButtonThumbnailTextureAltasOffsetMultiplierY;
		AssetButton.AssetParams.CreateDefaultAssetCollision = CreateAssetCollision;
		VoodooEngine::Engine->StoredButtonAssets.push_back(AssetButton);
	};
	void AddAssetContentForLevelEditor()
	{
		// Iterate over all stored game objects in the game and add them as asset buttons
		for (int i = 0; i < VoodooEngine::Engine->StoredGameObjectIDs.size(); ++i)
		{
			auto Iterator = VoodooEngine::Engine->StoredGameObjectIDs.find(i);

			AddLevelEditorAssetButton(
				Iterator->first,
				Iterator->second.CreateDefaultAssetCollision,
				Iterator->second.AssetFilePath,
				Iterator->second.AssetButtonThumbnailTextureAtlasHeight,
				Iterator->second.AssetButtonThumbnailTextureAtlasOffsetMultiplierY);
		}

		// When above iteration is done, store all the buttons in the level editor,
		// which will then be used as clickable buttons
		for (int i = 0; i < VoodooEngine::Engine->StoredButtonAssets.size(); ++i)
		{
			CurrentStoredButtonAssets.push_back(VoodooEngine::Engine->StoredButtonAssets[i]);
		}
	};
	bool ReachedLimitGridPrevious()
	{
		if (AssetIndexDisplayed.Min <= 0)
		{
			return true;
		}

		return false;
	};
	bool ReachedLimitGridNext()
	{
		if (AssetIndexDisplayed.Max > CurrentStoredButtonAssets.size())
		{
			return true;
		}

		return false;
	};
	void DisplayAssets(int ButtonPressed)
	{
		switch (ButtonPressed)
		{
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS:
			if (!ReachedLimitGridPrevious())
			{
				AssetIndexDisplayed.Min -= ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
				AssetIndexDisplayed.Max -= ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
			}
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT:
			if (!ReachedLimitGridNext())
			{
				AssetIndexDisplayed.Min += ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
				AssetIndexDisplayed.Max += ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
			}
			break;
		default:
			AssetIndexDisplayed.Min = 0;
			AssetIndexDisplayed.Max = ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
			break;
		}

		// Set all assets to hidden as default
		for (int i = 0; i < CurrentStoredButtonAssets.size(); ++i)
		{
			CurrentStoredButtonAssets[i].AssetButton->ButtonBitmap.BitmapParams.BitmapSetToNotRender = true;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.NoCollision = true;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.RenderCollisionRect = false;
		}

		// Only display desired assets
		for (int i = 0; i < CurrentStoredButtonAssets.size(); ++i)
		{
			if (i < AssetIndexDisplayed.Min)
			{
				continue;
			}
			// The subtraction of "1" is there to account for the vector starting at "0"
			if (i > AssetIndexDisplayed.Max - 1)
			{
				continue;
			}

			CurrentStoredButtonAssets[i].AssetButton->ButtonBitmap.BitmapParams.BitmapSetToNotRender = false;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.NoCollision = false;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.RenderCollisionRect = true;
		}
	};
};
*/
