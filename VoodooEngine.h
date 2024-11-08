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
extern "C" VOODOOENGINE_API void RenderCollisionRectangles(
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
};
// Level Assets
struct SAsset
{
	Button* AssetButton = nullptr;
	int AssetID = 0;
	const wchar_t* AssetPath = L"";

	bool operator==(const SAsset& Other) const 
	{
		return (AssetButton == Other.AssetButton &&
			AssetID == Other.AssetID);
	}
};
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
	std::vector<SAsset> StoredAssets;
	std::vector<Object*> StoredObjects;
	std::vector<UpdateComponent*> StoredUpdateComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<BitmapComponent*> StoredBitmapComponents;
	std::map<int, bool> StoredInputs;
	std::vector<InputCallback*> StoredInputCallbacks;

	// Font asset paths
	const wchar_t* DefaultFont = L"EngineContent/Font/FontMonogram.png";
	const wchar_t* DebugFont = L"EngineContent/Font/FontMonogramDebug.png";
	
	// This determines the letter space for any texts created
	int LetterSpace = 12;

	// Only used in editor mode
	std::vector<BitmapComponent*> StoredEditorBitmapComponents;
	// Used only for console debug print
	std::vector<BitmapComponent*> StoredScreenPrintTexts;
	// This keeps track of the number of console text prints have been printed
	// (Offsets a newly printed text down a column if text in column already has been printed)
	// Will reset once console window is deleted
	int ScreenColumnsPrinted = 0;
};
extern "C" VOODOOENGINE_API void CreateMouse(VoodooEngine* Engine, SVector MouseColliderSize);
extern "C" VOODOOENGINE_API void DeleteMouse(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize);
extern "C" VOODOOENGINE_API void UpdateMouseLocation(VoodooEngine* Engine, SVector NewLocation);
extern "C" VOODOOENGINE_API void UpdateCustomMouseCursor(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API bool HideSystemMouseCursor(UINT Message, LPARAM LParam);
extern "C" VOODOOENGINE_API void RenderCustomMouseCursor(ID2D1HwndRenderTarget* Renderer, VoodooEngine* Engine);

extern "C" VOODOOENGINE_API Button* CreateButton(
	VoodooEngine* Engine, Button* ButtonToCreate, ButtonParameters ButtonParams);
extern "C" VOODOOENGINE_API void DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete);
//-------------------------------------------

// Editor asset list
struct SEditorAssetList
{
	// Engine
	const wchar_t* GameIcon = L"EngineContent/Ico/GameIcon.ico";
	const wchar_t* EditorIcon = L"EngineContent/Ico/EditorIcon.ico";

	// Level editor
	const wchar_t* LevelEditorUI = L"EngineContent/LevelEditor/LevelEditorUI.png";
	const wchar_t* LevelEditorTileIconBase = L"EngineContent/LevelEditor/TileIconBase.png";
	const wchar_t* LevelEditorButtonDefaultW120 = L"EngineContent/LevelEditor/DefaultButtonW140.png";
};

// Create/Delete
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

#define BUTTON_ID_SAVELEVEL 1
#define BUTTON_ID_OPENLEVEL 2
#define BUTTON_ID_PLAYLEVEL 3
#define BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS 4
#define BUTTON_ID_SELECT_ASSET_LIST_NEXT 5

#define ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_1 0
#define ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_2 100
#define ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_3 200

#define ASSET_SELECTION_GRID_OFFSETLOC_ROW_1 0

// Always divide the "ASSETSDISPLAYED" by 3 
// and take the first 2 out of 3 results as the column 2-3 to get even columns of assets displayed 
#define ASSET_SELECTION_GRID_MAXNUM_ASSETSDISPLAYED 27
#define ASSET_SELECTION_GRID_MAXNUM_COLUMN_2 9
#define ASSET_SELECTION_GRID_MAXNUM_COLUMN_3 18

#define BUTTON_LOC_X_PLAYLEVEL 890
#define BUTTON_LOC_Y_PLAYLEVEL 20
#define BUTTON_LOC_X_PREVIOUS 1615
#define BUTTON_LOC_X_NEXT 1770
#define BUTTON_LOC_Y_NEXT_PREVIOUS 100

#define BUTTON_LOC_X_ORIGIN_ASSET 1615
#define BUTTON_LOC_Y_ORIGIN_ASSET 165

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
		PlayLevelButton = CreateLevelEditorButton(TwoSided,
			PlayLevelButton, "play_level", 
			BUTTON_ID_PLAYLEVEL, { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL }, 
			Asset.LevelEditorButtonDefaultW120);
		PreviousButton = CreateLevelEditorButton(TwoSided,
			PreviousButton, "previous",
			BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS, { BUTTON_LOC_X_PREVIOUS, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonDefaultW120);
		NextButton = CreateLevelEditorButton(TwoSided,
			NextButton, "next",
			BUTTON_ID_SELECT_ASSET_LIST_NEXT, { BUTTON_LOC_X_NEXT, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonDefaultW120);

		// Create default asset ID list of clickable asset buttons
		CreateAssetButtons(StartAssetID);
	}

	int StartAssetID = 0;
	std::vector<SAsset> CurrentStoredAssets;
	BitmapComponent* LevelEditorUI = nullptr;
	Button* PlayLevelButton = nullptr;
	Button* PreviousButton = nullptr;
	Button* NextButton = nullptr;

	void CreateAssetButtons(int StartAssetID)
	{
		if (!PopulateCurrentStoredAssets(StartAssetID))
		{
			return;
		}

		float LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_1;
		float LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
		float OffsetYAmount = 100;

		int Index = -1;
		for (int i = 0; i < CurrentStoredAssets.size(); i++)
		{
			Index++;

			// Move to next column if at max amount of assets displayed at column 1
			if (Index == ASSET_SELECTION_GRID_MAXNUM_COLUMN_2)
			{
				LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_2;
				LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
			}
			// Move to next column if at max amount of assets displayed at column 2
			else if (Index == ASSET_SELECTION_GRID_MAXNUM_COLUMN_3)
			{
				LocXOffset = ASSET_SELECTION_GRID_OFFSETLOC_COLUMN_3;
				LocYOffset = ASSET_SELECTION_GRID_OFFSETLOC_ROW_1;
			}
			// Stop creating buttons after reaching limit of buttons that can be displayed (final row)
			if (Index < ASSET_SELECTION_GRID_MAXNUM_ASSETSDISPLAYED)
			{
				int CurrentAssetID = CurrentStoredAssets[i].AssetID;
				Button* AssetButton = nullptr;
				AssetButton = CreateLevelEditorButton(OneSided, AssetButton, "", CurrentAssetID,
					{ BUTTON_LOC_X_ORIGIN_ASSET + LocXOffset, (BUTTON_LOC_Y_ORIGIN_ASSET + LocYOffset) },
					CurrentStoredAssets[i].AssetPath);
				CurrentStoredAssets[i].AssetButton = AssetButton;
				LocYOffset += OffsetYAmount;
			}
		}
	};
	void DeleteAssetButtons()
	{
		// Delete both components created with "new": bitmapcomponent, collisioncomponent
		// And finally delete button created with "new"
		for (int i = 0; i < CurrentStoredAssets.size(); i++)
		{
			while (!CurrentStoredAssets.empty())
			{
				if (!CurrentStoredAssets[i].AssetButton)
				{
					return;
				}

				SAsset AssetToDelete = CurrentStoredAssets[i];

				CurrentStoredAssets.erase(std::remove(CurrentStoredAssets.begin(),
					CurrentStoredAssets.end(), AssetToDelete));

				Engine->StoredEditorBitmapComponents.erase(std::remove(
					Engine->StoredEditorBitmapComponents.begin(),
					Engine->StoredEditorBitmapComponents.end(), AssetToDelete.AssetButton->ButtonBitmap));

				Engine->StoredCollisionComponents.erase(std::remove(
					Engine->StoredCollisionComponents.begin(),
					Engine->StoredCollisionComponents.end(), AssetToDelete.AssetButton->ButtonCollider));
					
				delete AssetToDelete.AssetButton->ButtonBitmap;
				delete AssetToDelete.AssetButton->ButtonCollider;
				delete AssetToDelete.AssetButton;
			}
		}

		CurrentStoredAssets.clear();

		if (CurrentStoredAssets.empty())
		{
			ScreenPrint("assets_empty", Engine);
		}
	};
	Button* CreateLevelEditorButton(EButtonType ButtonType,
		Button* ButtonToCreate, std::string ButtonName, int ButtonID,
		SVector ButtonLocation, const wchar_t* AssetPath)
	{
		ButtonToCreate = new Button();
		ButtonToCreate->ButtonParams.ButtonType = ButtonType;
		ButtonToCreate->ButtonParams.AssetPathButtonBitmap = AssetPath;
		ButtonToCreate->ButtonParams.ButtonLocation = ButtonLocation;
		ButtonToCreate->ButtonParams.ButtonCollisionTag = ButtonID;
		ButtonToCreate->ButtonParams.ButtonTextString = ButtonName;
		ButtonToCreate = CreateButton(Engine, ButtonToCreate, ButtonToCreate->ButtonParams);

		return ButtonToCreate;
	};
	void ResetButtonsBitmapSource(Button* ButtonBitmapToReset)
	{
		// Reset to the default bitmap source location (if two sided e.g. pressed/not pressed)
		SetBitmapSourceLocationX(ButtonBitmapToReset->ButtonBitmap,
			ButtonBitmapToReset->ButtonBitmap->Bitmap->GetSize().width / 2);
	};
	void SetButtonBitmapSourceClicked(Button* ButtonBitmapToSet)
	{
		// This shifts the bitmap source of the button to render the "pressed" version of the button
		int ButtonClickedSourceLocationOffsetX = 2;

		SetBitmapSourceLocationX(
			ButtonBitmapToSet->ButtonBitmap,
			ButtonBitmapToSet->ButtonBitmap->Bitmap->GetSize().width / 2,
			ButtonClickedSourceLocationOffsetX);
	};
	void OnButtonPressed()
	{
		switch (ButtonHoveredID)
		{
		case BUTTON_ID_SAVELEVEL:
			break;
		case BUTTON_ID_OPENLEVEL:
			break;
		case BUTTON_ID_PLAYLEVEL:
			SetButtonBitmapSourceClicked(PlayLevelButton);
			ScreenPrint("pressed_button_play_level", Engine);
			break;
		case BUTTON_ID_SELECT_ASSET_LIST_PREVIOUS:
			SetButtonBitmapSourceClicked(PreviousButton);
			//ScreenPrint("pressed_button_previous", Engine);
			DeleteAssetButtons();
			// Make it a multiplier instead that decreases (and stops if limit has reached)
			StartAssetID -= 27;
			CreateAssetButtons(StartAssetID);
			break;
		case BUTTON_ID_SELECT_ASSET_LIST_NEXT:
			SetButtonBitmapSourceClicked(NextButton);
			//ScreenPrint("pressed_button_next", Engine);
			DeleteAssetButtons();
			// Make it a multiplier instead that increases (and stops if limit has reached)
			StartAssetID += 27;
			CreateAssetButtons(StartAssetID);
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

		if (ButtonHoveredID == 0)
		{
			return;
		}

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
	void Update(float DeltaTime)
	{
		if (!Engine)
		{
			return;
		}

		CheckForCollision(this, PlayLevelButton->ButtonCollider, Engine->Mouse.MouseCollider);
		CheckForCollision(this, PreviousButton->ButtonCollider, Engine->Mouse.MouseCollider);
		CheckForCollision(this, NextButton->ButtonCollider, Engine->Mouse.MouseCollider);
		
		for (int i = 0; i < CurrentStoredAssets.size(); i++)
		{
			if (!CurrentStoredAssets[i].AssetButton)
			{
				continue;
			}

			CheckForCollision(
				this, CurrentStoredAssets[i].AssetButton->ButtonCollider, 
				Engine->Mouse.MouseCollider);
		}
	};
	void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag)
	{
		ButtonHoveredID = SenderCollisionTag;
	};
	void OnEndOverlap(int SenderCollisionTag, int TargetCollisionTag)
	{
		if (SenderCollisionTag == ButtonHoveredID)
		{
			ButtonHoveredID = 0;
		}
	};

private:
	VoodooEngine* Engine = nullptr;
	SEditorAssetList Asset;
	int ButtonHoveredID = 0;

	bool PopulateCurrentStoredAssets(int StartAssetID)
	{
		if (StartAssetID > Engine->StoredAssets.size())
		{
			return false;
		}

		for (int i = StartAssetID; i < Engine->StoredAssets.size(); i++)
		{
			CurrentStoredAssets.push_back(Engine->StoredAssets[i]);
		}

		return true;
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
