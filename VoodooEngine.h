#pragma once

// STL library
#include <map>
#include <string>
#include <vector>
#include <iostream>

// Win32 API
#include <Windows.h>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

// Direct2D API
#include <d2d1.h>
#include <d2d1_1.h>
#pragma comment(lib, "d2d1.lib")

#ifdef VOODOOENGINE_EXPORTS
#define VOODOOENGINE_API __declspec(dllexport)
#else
#define VOODOOENGINE_API __declspec(dllimport)
#endif

#define INPUT_PRIMARYMOUSE_PRESSED 1
#define INPUT_PRIMARYMOUSE_RELEASED 2
#define INPUT_SECONDARYMOUSE_PRESSED 3
#define INPUT_SECONDARYMOUSE_RELEASED 4

// Window
//-------------------------------------------
// Window parameters information
struct SWindowParams
{
	HWND HWind = nullptr;
	WNDCLASSEX WindowClass;
	LPCWSTR WindowTitle;
	int ScreenResolutionWidth;
	int ScreenResolutionHeight;
	int Fullscreen;
};
// Create window and return window handle
extern "C" VOODOOENGINE_API void CreateAppWindow(
	SWindowParams &WindowParams, WNDPROC InputCallbackFunction);
// This will set a custom assigned icon of the app window title/task bar
extern "C" VOODOOENGINE_API void SetCustomAppIcon(LPCWSTR IconFileName, HWND &HWind);
// Updates any changes made to the app window (e.g. dragging the window)
extern "C" VOODOOENGINE_API void UpdateAppWindow();
//-------------------------------------------

// Input
//-------------------------------------------
// Any class that inherits "InputCallback" class 
// will be able to recieve a broadcast of which input was pressed/released
class InputCallback
{
public:
	virtual void BroadcastInput(int Input, bool Pressed){};
};
// Check if input is being pressed/released, returns a bool and is false as default if no input is found
extern "C" VOODOOENGINE_API bool InputPressed(std::map<int, bool> StoredInputs, int InputToCheck);
// Sends input broadcast to all inherited "InputCallback" classes whenever an input is pressed 
extern "C" VOODOOENGINE_API void BroadcastInput(std::vector<InputCallback*> StoredCallbacks, int Input, bool Pressed);
//-------------------------------------------

// Utility
//-------------------------------------------
// 2D vector
struct SVector
{
	float X = 0;
	float Y = 0;
};
// Color (RGB)
struct SColor
{
	int R = 255;
	int G = 255;
	int B = 255;
};
//-------------------------------------------

// Base component that all movable components inherit
// This tracks the component location relative to its owner (the object it´s attached to)
//-------------------------------------------
class TransformComponent
{
public:
	SVector ComponentLocation = { 0, 0 };
};
//-------------------------------------------

// Bitmap
//-------------------------------------------
// Bitmap information
struct SBitmap
{
	int RenderLayer = 0;
	float Opacity = 1;
	bool HiddenInGame = false;
	SVector BitmapOffsetLeft = { 0, 0 };
	SVector BitmapOffsetRight = { 0, 0 };
	SVector BitmapSource = { 0, 0 };
};
// Bitmap component
class BitmapComponent : public TransformComponent
{
public:
	ID2D1Bitmap* Bitmap = nullptr;
	SBitmap BitmapParams = {};
};
// Create bitmap image 
extern "C" VOODOOENGINE_API ID2D1Bitmap* CreateNewBitmap(
	ID2D1HwndRenderTarget* RenderTarget, const wchar_t* FileName, bool Flip = false);
// Setup bitmap struct
extern "C" VOODOOENGINE_API SBitmap SetupBitmapParams(ID2D1Bitmap* CreatedBitmap);
// Offset bitmap
extern "C" VOODOOENGINE_API void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int LocationOffsetMultiplier = 1);
//-------------------------------------------

// Animation
//-------------------------------------------
// Contains frame size/total frames and animation speed/state info, 
// used in conjunction with animation component
struct AnimationParameters
{
	int AnimationState = 1;
	float AnimationSpeed = 1;
	int TotalFrames = 1;
	int FrameWidth = 0;
	int FrameHeight = 0;
	int CurrentFrame = 1;
	float AnimationTimer = 0;
};
// Animation component class
class AnimationComponent
{
public:
	AnimationParameters AnimationParams = {};
};
// Get animation statess
extern "C" VOODOOENGINE_API void UpdateAnimationState(
	AnimationParameters &AnimationParams,
	SVector &BitmapSource,
	SVector &BitmapOffsetLeft,
	SVector &BitmapOffsetRight);
// Update animation
extern "C" VOODOOENGINE_API void UpdateAnimation(
	AnimationParameters &AnimationParams,
	SVector &BitmapSource,
	SVector &BitmapOffsetLeft,
	SVector &BitmapOffsetRight,
	float DeltaTime);
//-------------------------------------------

// Object
//-------------------------------------------
// Base object class inherited by all objects in the engine that will be transformed 
// and/or needs collision overlap events
//-------------------------------------------
class Object
{
public:
	SVector Location = { 0, 0 };

	virtual void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag){};
	virtual void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag){};
};
extern "C" VOODOOENGINE_API SVector GetObjectLocation(Object* Object);
extern "C" VOODOOENGINE_API void SetObjectLocation(Object* Object, SVector NewLocation);
extern "C" VOODOOENGINE_API void SetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component, SVector NewLocation);
extern "C" VOODOOENGINE_API SVector GetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component);
//-------------------------------------------

// Collision detection
//-------------------------------------------
// Collision component class
class CollisionComponent : public TransformComponent
{
public:
	bool NoCollision = false;
	bool IsOverlapped = false;
	bool RenderCollisionRect = false;
	int CollisionTag = 0;
	std::vector<int> CollisionTagsToIgnore;
	SColor CollisionRectColor = { 255, 255, 255 };
	SVector CollisionRect = { 0, 0 };
	SVector CollisionRectOffset = { 0, 0 };
};
extern "C" VOODOOENGINE_API bool IsCollisionDetected(
	CollisionComponent* Sender, CollisionComponent* Target);
extern "C" VOODOOENGINE_API void CheckForCollision(
	Object* CallbackOwner, CollisionComponent* Sender, CollisionComponent* Target);
extern "C" VOODOOENGINE_API void CheckForCollisionMultiple(
	Object* CallbackOwner, CollisionComponent* Sender, std::vector<CollisionComponent*> Targets);
//-------------------------------------------

// Rendering
//-------------------------------------------
extern "C" VOODOOENGINE_API ID2D1HwndRenderTarget* SetupRenderer(
	ID2D1HwndRenderTarget* RenderTarget, HWND WindowHandle);
extern "C" VOODOOENGINE_API void RenderBitmapByLayer(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> StoredBitmaps, int RenderLayer);
extern "C" VOODOOENGINE_API void RenderBitmaps(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> BitmapsToRender, 
	int MaxNumRenderLayers = 0);
extern "C" VOODOOENGINE_API void RenderCollisionRectangleMultiple(
	ID2D1HwndRenderTarget* Renderer, std::vector<CollisionComponent*> CollisionRectsToRender);
extern "C" VOODOOENGINE_API void RenderCollisionRectangle(
	ID2D1HwndRenderTarget* Renderer, CollisionComponent* CollisionRectangleToRender);
//-------------------------------------------

// Update component inherited in all objects that needs to update each frame
//-------------------------------------------
class UpdateComponent
{
public:
	virtual void Update(float DeltaTime) = 0;
};
//-------------------------------------------

// Mouse
//-------------------------------------------
class VoodooMouse : public Object
{
public:
	BitmapComponent* MouseBitmap = nullptr;
	CollisionComponent* MouseCollider = nullptr;
	bool PrimaryMousePressed = false;
	bool SecondaryMousePressed = false;
};
//-------------------------------------------

// Voodoo engine
//-------------------------------------------
// Button
enum EButtonType
{
	TwoSided,
	OneSided
};
struct ButtonParameters
{
	EButtonType ButtonType = TwoSided;
	const wchar_t* AssetPathButtonBitmap = L"";
	int ButtonCollisionTag = 0;
	std::string ButtonTextString = "";
	SVector ButtonTextOffset = { -2, 10 };
	SVector ButtonLocation = { 0, 0 };
};
class Button
{
public:
	CollisionComponent* ButtonCollider = nullptr;
	BitmapComponent* ButtonBitmap = nullptr;
	ButtonParameters ButtonParams = {};
	std::vector<BitmapComponent*> ButtonText;
};
// Level Assets
struct SAssetButton
{
	Button* AssetButton = nullptr;
	int AssetID = 0;
	const wchar_t* AssetPath = L"";

	bool operator==(const SAssetButton& Other) const 
	{
		return (AssetButton == Other.AssetButton);
	}
};
// Game object 
//-------------------------------------------
// This class is used for all objects placed in levels
class GameObject : public Object, public BitmapComponent, public CollisionComponent
{
public:
	// Default constructor
	GameObject(){};
	// Constructor used
	GameObject(SVector SpawnLocation)
	{
		Location = SpawnLocation;
	};
	BitmapComponent* GameObjectBitmap = nullptr;
	int GameObjectNumberID = 0;

	// Optional, can be set to not be created
	CollisionComponent* AssetCollision = nullptr;
	bool CreateDefaultAssetCollisionInGame = false;
};
//-------------------------------------------
// Engine class
class VoodooEngine
{
public:
	VoodooMouse Mouse;
	bool DebugMode = false;
	bool EditorMode = false;
	bool EngineRunning = false;
	SWindowParams Window;
	ID2D1HwndRenderTarget* Renderer = nullptr;
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER EndCounter;
	LARGE_INTEGER Counts;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER FPS;
	float TargetSecondsPerFrame = 1 / 80;
	float DeltaTime = 0;
	std::vector<UpdateComponent*> StoredUpdateComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<BitmapComponent*> StoredBitmapComponents;
	std::map<int, bool> StoredInputs;
	std::vector<InputCallback*> StoredInputCallbacks;
	std::vector<GameObject*> StoredGameObjects;
	std::map<int, const wchar_t*> StoredGameObjectIDs;
	std::vector<SAssetButton> StoredButtonAssets;

	// Font asset paths
	const wchar_t* DefaultFont = L"EngineContent/Font/FontMonogram.png";
	const wchar_t* DebugFont = L"EngineContent/Font/FontMonogramDebug.png";
	
	// This determines the letter space for any texts created
	int LetterSpace = 12;

	// Used to store buttons information 
	// (can be used for both in game UI and editor UI buttons)
	std::vector<BitmapComponent*> StoredButtonBitmapComponents;
	std::vector<BitmapComponent*> StoredButtonTexts;
	std::vector<CollisionComponent*> StoredButtonCollisionComponents;

	// Default collision rect color for editor mode assets
	SColor EditorCollisionRectColor = { 200, 0, 255 };

	// Only used in editor mode
	std::vector<BitmapComponent*> StoredEditorBitmapComponents;
	// Used only for screen debug print
	std::vector<BitmapComponent*> StoredScreenPrintTexts;
	// This keeps track of the number of console text prints have been printed
	// (Offsets a newly printed text down a column if text in column already has been printed)
	// Will reset once console window is deleted
	int ScreenPrintTextColumnsPrinted = 0;

	template<class T>
	T* CreateGameObject(
		T* ClassToSpawn, SVector SpawnLocation, int AssetID, bool CreateAssetCollision = false)
	{
		auto Iterator = StoredGameObjectIDs.find(AssetID);
		if (Iterator == StoredGameObjectIDs.end())
		{
			ClassToSpawn = nullptr;
			return ClassToSpawn;
		}

		const wchar_t* AssetPath = Iterator->second;

		ClassToSpawn = new T(SpawnLocation);
		ClassToSpawn->Location = SpawnLocation;
		ClassToSpawn->GameObjectNumberID = AssetID;
		ClassToSpawn->CreateDefaultAssetCollisionInGame = CreateAssetCollision;
		ClassToSpawn->GameObjectBitmap = new BitmapComponent();
		ClassToSpawn->GameObjectBitmap->Bitmap = CreateNewBitmap(Renderer, AssetPath);
		ClassToSpawn->GameObjectBitmap->BitmapParams = 
			SetupBitmapParams(ClassToSpawn->GameObjectBitmap->Bitmap);
		ClassToSpawn->GameObjectBitmap->ComponentLocation = SpawnLocation;
		StoredBitmapComponents.push_back(ClassToSpawn->GameObjectBitmap);

		// If in editor mode, create a clickable collision rect for the spawned game object 
		// in order for it to be selectable in the level editor
		if (EditorMode || CreateAssetCollision)
		{
			SVector GameObjectBitmapSize = 
				{ ClassToSpawn->GameObjectBitmap->Bitmap->GetSize().width,
				ClassToSpawn->GameObjectBitmap->Bitmap->GetSize().height };
			ClassToSpawn->AssetCollision = new CollisionComponent();
			ClassToSpawn->AssetCollision->CollisionRect = GameObjectBitmapSize;
			ClassToSpawn->AssetCollision->ComponentLocation = SpawnLocation;
			ClassToSpawn->AssetCollision->CollisionTag = AssetID;
			// Only set to render collision rect if in debug mode
			if (DebugMode)
			{
				ClassToSpawn->AssetCollision->RenderCollisionRect = true;
				ClassToSpawn->AssetCollision->CollisionRectColor = EditorCollisionRectColor;
			}
			StoredCollisionComponents.push_back(ClassToSpawn->AssetCollision);
		}
		StoredGameObjects.push_back(ClassToSpawn);

		return ClassToSpawn;
	};
};
extern "C" VOODOOENGINE_API void CreateMouse(VoodooEngine* Engine, SVector MouseColliderSize);
extern "C" VOODOOENGINE_API void DeleteMouse(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize);
extern "C" VOODOOENGINE_API void UpdateMouseLocation(VoodooEngine* Engine, SVector NewLocation);
extern "C" VOODOOENGINE_API void UpdateCustomMouseCursor(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API bool HideSystemMouseCursor(UINT Message, LPARAM LParam);
extern "C" VOODOOENGINE_API void RenderCustomMouseCursor(
	ID2D1HwndRenderTarget* Renderer, VoodooEngine* Engine);

extern "C" VOODOOENGINE_API Button* CreateButton(VoodooEngine* Engine, 
	EButtonType ButtonType, Button* ButtonToCreate, std::string ButtonName, int ButtonID,
	SVector ButtonLocation, const wchar_t* AssetPath);
extern "C" VOODOOENGINE_API void DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete);
//-------------------------------------------

// Editor asset list
struct SEditorAssetList
{
	// Engine
	const wchar_t* GameIcon = L"EngineContent/Ico/GameIcon.ico";
	const wchar_t* EditorIcon = L"EngineContent/Ico/EditorIcon.ico";

	// Used for debug only 
	const wchar_t* AnimationSpritesDebug = L"EngineContent/Debug/AnimationTesting.png";

	// Level editor
	const wchar_t* LevelEditorUI = L"EngineContent/LevelEditor/LevelEditorUI.png";
	const wchar_t* LevelEditorTileIconBase = L"EngineContent/LevelEditor/TileIconBase.png";
	const wchar_t* LevelEditorButtonDefaultW140 = 
		L"EngineContent/LevelEditor/DefaultButtonW140.png";
	const wchar_t* LevelEditorButtonActivateDeactivateW140 = 
		L"EngineContent/LevelEditor/ActivateDeactivateButtonW140.png";
};

// Delete functions
//-------------------------------------------
extern "C" VOODOOENGINE_API void DeleteBitmapComponent(VoodooEngine* Engine, BitmapComponent* Component);
extern "C" VOODOOENGINE_API void DeleteCollisionComponent(VoodooEngine* Engine, CollisionComponent* Component);
extern "C" VOODOOENGINE_API void ScreenPrint(std::string DebugText, VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void ClearScreenPrint(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void CloseApp(VoodooEngine* Engine);
//-------------------------------------------

// Level Editor
//-------------------------------------------
class LevelEditor : public Object, public UpdateComponent, public InputCallback
{

#define BUTTON_ID_SAVELEVEL -1
#define BUTTON_ID_OPENLEVEL -2
#define BUTTON_ID_PLAYLEVEL -3
#define BUTTON_ID_STOPPLAY -4
#define BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS -5
#define BUTTON_ID_SELECT_ASSET_LIST_NEXT -6

#define BUTTON_LOC_X_PLAYLEVEL 890
#define BUTTON_LOC_Y_PLAYLEVEL 20
#define BUTTON_LOC_X_PREVIOUS 1615
#define BUTTON_LOC_X_NEXT 1770
#define BUTTON_LOC_Y_NEXT_PREVIOUS 100

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

public:
	LevelEditor(VoodooEngine* EngineReference)
	{
		Engine = EngineReference;

		// Add level editor to the list of objects that calls update function every frame
		Engine->StoredUpdateComponents.push_back(this);

		// Add input callback for level editor
		Engine->StoredInputCallbacks.push_back(this);

		// Create level editor UI
		LevelEditorUI = new BitmapComponent;
		LevelEditorUI->Bitmap = CreateNewBitmap(Engine->Renderer, Asset.LevelEditorUI);
		LevelEditorUI->BitmapParams = SetupBitmapParams(LevelEditorUI->Bitmap);
		Engine->StoredEditorBitmapComponents.push_back(LevelEditorUI);

		// Create all the clickable level editor buttons
		CreatePlayLevelButton();
		PreviousButton = CreateButton(Engine, TwoSided,
			PreviousButton, "previous",
			BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS, { BUTTON_LOC_X_PREVIOUS, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonDefaultW140);
		NextButton = CreateButton(Engine, TwoSided,
			NextButton, "next",
			BUTTON_ID_SELECT_ASSET_LIST_NEXT, { BUTTON_LOC_X_NEXT, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonDefaultW140);

		// Create all clickable assets buttons
		CreateAssetButtons();
		// Display default asssets
		DisplayAssets(0);
	}

	BitmapComponent* LevelEditorUI = nullptr;
	void(*ButtonPressedCallback)(int);

	void CreatePlayLevelButton()
	{
		PlayLevelButton = CreateButton(Engine, TwoSided,
			PlayLevelButton, "play_level",
			BUTTON_ID_PLAYLEVEL, { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonDefaultW140);
	};
	void CreateStopPlayButton()
	{
		StopPlayButton = CreateButton(Engine, TwoSided,
			StopPlayButton, "stop_play",
			BUTTON_ID_STOPPLAY, { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonActivateDeactivateW140);

		SetBitmapSourceLocationX(
			StopPlayButton->ButtonBitmap,
			StopPlayButton->ButtonBitmap->Bitmap->GetSize().width / 2, 
			2);
	};
	void CreateAssetButtons()
	{
		PopulateCurrentStoredAssets();

		float LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_1;
		float LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
		float OffsetYAmount = 100;

		int Index = 0;

		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
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
			AssetButton = CreateButton(Engine, OneSided, AssetButton, "", CurrentAssetID,
				{ ASSET_SELECTION_BUTTON_LOC_X_ORIGIN + LocXOffset, 
				(ASSET_SELECTION_BUTTON_LOC_Y_ORIGIN + LocYOffset) },
				CurrentStoredButtonAssets[i].AssetPath);
				
			CurrentStoredButtonAssets[i].AssetButton = AssetButton;
			LocYOffset += OffsetYAmount;

			Index++;
		}
	};
	void DeletePlayLevelButton()
	{
		DeleteButton(Engine, PlayLevelButton);
		PlayLevelButton = nullptr;
		// Reset button ID to reference no editor button
		ButtonID = 0;
	};
	void DeleteStopPlayButton()
	{
		DeleteButton(Engine, StopPlayButton);
		StopPlayButton = nullptr;
		// Reset button ID to reference no editor button
		ButtonID = 0;
	};
	void DeleteAssetButtons()
	{
		// Remove assets from vector
		// delete both components created with "new": bitmapcomponent, collisioncomponent
		// And finally delete button created with "new"
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
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

				Engine->StoredButtonBitmapComponents.erase(std::remove(
					Engine->StoredButtonBitmapComponents.begin(),
					Engine->StoredButtonBitmapComponents.end(), AssetToDelete.AssetButton->ButtonBitmap));
				
				Engine->StoredButtonCollisionComponents.erase(std::remove(
					Engine->StoredButtonCollisionComponents.begin(),
					Engine->StoredButtonCollisionComponents.end(), AssetToDelete.AssetButton->ButtonCollider));

				delete AssetToDelete.AssetButton->ButtonBitmap;
				delete AssetToDelete.AssetButton->ButtonCollider;
				delete AssetToDelete.AssetButton;
			}
		}

		CurrentStoredButtonAssets.clear();

		if (CurrentStoredButtonAssets.empty())
		{
			ScreenPrint("assets_empty", Engine);
		}
	};
	void ResetButtonsBitmapSource(Button* ButtonBitmapToReset)
	{
		if (!ButtonBitmapToReset)
		{
			return;
		}

		// Reset to the default bitmap source location (if two sided e.g. pressed/not pressed)
		SetBitmapSourceLocationX(ButtonBitmapToReset->ButtonBitmap,
			ButtonBitmapToReset->ButtonBitmap->Bitmap->GetSize().width / 2);
	};
	void SetButtonBitmapSourceClicked(Button* ButtonBitmapToSet)
	{
		if (!ButtonBitmapToSet)
		{
			return;
		}

		// This shifts the bitmap source of the button to render the "pressed" version of the button
		int ButtonClickedSourceLocationOffsetX = 2;

		SetBitmapSourceLocationX(
			ButtonBitmapToSet->ButtonBitmap,
			ButtonBitmapToSet->ButtonBitmap->Bitmap->GetSize().width / 2,
			ButtonClickedSourceLocationOffsetX);
	};
	void OnButtonPressed()
	{
		ButtonPressedCallback(ButtonID);

		switch (ButtonID)
		{
		case BUTTON_ID_SAVELEVEL:
			break;
		case BUTTON_ID_OPENLEVEL:
			break;
		case BUTTON_ID_PLAYLEVEL:
			DeletePlayLevelButton();
			CreateStopPlayButton();
			break;
		case BUTTON_ID_STOPPLAY:
			DeleteStopPlayButton();
			CreatePlayLevelButton();
			break;
		case BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS:
			SetButtonBitmapSourceClicked(PreviousButton);
			DisplayAssets(BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS);
			break;
		case BUTTON_ID_SELECT_ASSET_LIST_NEXT:
			SetButtonBitmapSourceClicked(NextButton);
			DisplayAssets(BUTTON_ID_SELECT_ASSET_LIST_NEXT);
			break;
		// USED FOR TESTING ONLY 
		// OBS!!
		// (MAKE A BINDABLE BROADCAST WHEN A BUTTON IS PRESSED AND PASS BUTTON ID AS PARAM,
		// THIS WAY ALL THE LOGIC CAN BE CUSTOMIZED PER GAME 
		// E.G. SPAWNING SPECIFIC GAME ASSETS WHEN CLICKING ON AN ASSET BUTTON, SAVING A SPECIFIC LEVEL)
		case 50:
			ScreenPrint("pressed_red_baby", Engine);
			break;
		case 51:
			ScreenPrint("pressed_blue_baby", Engine);
			break;
		case 52:
			ScreenPrint("pressed_green_baby", Engine);
			break;
		}
	};
	void BroadcastInput(int Input, bool Pressed)
	{
		ResetButtonsBitmapSource(PlayLevelButton);
		ResetButtonsBitmapSource(PreviousButton);
		ResetButtonsBitmapSource(NextButton);

		switch (Input)
		{
		case INPUT_PRIMARYMOUSE_PRESSED:
			if (Pressed)
			{
				OnButtonPressed();
			}
			break;
		case INPUT_SECONDARYMOUSE_PRESSED:
			if (Pressed)
			{

			}
			break;
		default:
			break;
		}
	};
	void UpdateButtonCollisionCheck(Button* ButtonToUpdate)
	{
		if (!ButtonToUpdate)
		{
			return;
		}

		CheckForCollision(this, ButtonToUpdate->ButtonCollider, Engine->Mouse.MouseCollider);
	};
	void Update(float DeltaTime)
	{
		if (!Engine)
		{
			return;
		}

		UpdateButtonCollisionCheck(PlayLevelButton);
		UpdateButtonCollisionCheck(StopPlayButton);
		UpdateButtonCollisionCheck(PreviousButton);
		UpdateButtonCollisionCheck(NextButton);
		
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
		{
			if (!CurrentStoredButtonAssets[i].AssetButton)
			{
				continue;
			}

			CheckForCollision(
				this, CurrentStoredButtonAssets[i].AssetButton->ButtonCollider, 
				Engine->Mouse.MouseCollider);
		}
	};
	void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag)
	{
		ButtonID = SenderCollisionTag;

		ScreenPrint("collision_detected", Engine);
	};
	void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag)
	{
		if (SenderCollisionTag == ButtonID)
		{
			ButtonID = 0;
		}
	};

private:
	struct AssetIndex
	{
		int Min = 0;
		int Max = ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
	};
	VoodooEngine* Engine = nullptr;
	int ButtonID = 0;
	SEditorAssetList Asset;
	AssetIndex AssetIndexDisplayed;
	std::vector<SAssetButton> CurrentStoredButtonAssets;
	Button* PlayLevelButton = nullptr;
	Button* StopPlayButton = nullptr;
	Button* PreviousButton = nullptr;
	Button* NextButton = nullptr;

	void PopulateCurrentStoredAssets()
	{
		for (int i = 0; i < Engine->StoredButtonAssets.size(); i++)
		{
			CurrentStoredButtonAssets.push_back(Engine->StoredButtonAssets[i]);
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
		case BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS:
			if (!ReachedLimitGridPrevious())
			{
				AssetIndexDisplayed.Min -= ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
				AssetIndexDisplayed.Max -= ASSET_SELECTION_GRID_MAXNUM_DISPLAYED;
			}
			break;
		case BUTTON_ID_SELECT_ASSET_LIST_NEXT:
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
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
		{
			CurrentStoredButtonAssets[i].AssetButton->ButtonBitmap->BitmapParams.HiddenInGame = true;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider->NoCollision = true;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider->RenderCollisionRect = false;
		}

		// Only display desired assets
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
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

			CurrentStoredButtonAssets[i].AssetButton->ButtonBitmap->BitmapParams.HiddenInGame = false;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider->NoCollision = false;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider->RenderCollisionRect = true;
		}
	};
};
//-------------------------------------------

// Update function that calls update to all connected update components
//-------------------------------------------
extern "C" VOODOOENGINE_API void Update(VoodooEngine* Engine, float DeltaTime);
//-------------------------------------------

// Frame rate related
//-------------------------------------------
extern "C" VOODOOENGINE_API float GetSecondsPerFrame(
	LARGE_INTEGER* StartCounter, LARGE_INTEGER* EndCounter, LARGE_INTEGER* Frequency);
extern "C" VOODOOENGINE_API void SetFPSLimit(VoodooEngine* Engine, float FPSLimit);
extern "C" VOODOOENGINE_API float UpdateFrameRate(VoodooEngine* Engine);
//-------------------------------------------

// File I/O
//-------------------------------------------
// Read only
extern "C" VOODOOENGINE_API bool UpdateDebugMode();
extern "C" VOODOOENGINE_API bool UpdateEditorMode();
// Read/Write (saving)
// Add functions here
//-------------------------------------------
