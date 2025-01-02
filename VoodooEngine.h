#pragma once

// VoodooEngine is a complete engine framework for making a 2D game
// --------------------
// Supported features:
// 
// Graphics
// - Creation of an application window with custom title name and icon using "Win32 API"
// - 2D bitmap rendering using "Direct2D API"
// - 2D spritesheet animation
// 
// Gameplay
// - User input for keyboard (using "Win32 API") and gamepad (using "XInput API") 
// - Collision detection using "AABB" algorithm
// - Playing one shot sounds/looping sounds using "XAudio2 API"
// - Update function with deltatime
// 
// Tools
// - Level editor 
// - Saving/loading from files

// Naming conventions
// --------------------
// Prefixes
// 
// - "S" stands for "Struct" e.g. "SVector", "SColor"
// - "E" stands for "enum" e.g. "EButtonType"
// - When a function name has prefix "Update" e.g. "UpdateAnimation", then it will be called every frame

// STL library
#include <map>
#include <string>
#include <vector>

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

// Global collision tags used by the editor
#define TAG_LEVEL_EDITOR_GIZMO -1
#define TAG_LEVEL_EDITOR_BUTTON_ID_NONE -2
#define TAG_LEVEL_EDITOR_BUTTON_SAVELEVEL -3
#define TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL -4
#define TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL -5
#define TAG_LEVEL_EDITOR_BUTTON_STOPPLAY -6
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS -7
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT -8

// Input
//---------------------
// Any class that inherit "InputCallback" class, 
// will be able to recieve a broadcast of which input was pressed/released
class InputCallback
{
public:
	// Pure virtual function since there is no reason to make it optional,
	// if a class inherit "InputCallback" then it will want to listen to the input broadcast
	virtual void InputBroadcast(int Input, bool Pressed) = 0;
};
// Check if input is being pressed/released, returns (not pressed) false as default if no input is found
extern "C" VOODOOENGINE_API bool InputPressed(std::map<int, bool> StoredInputs, int InputToCheck);
// Sends input broadcast to all listeners (contained by the vector) whenever an input is pressed
// Note that input ID of primary mouse is "1" and for secondary mouse "2", 
// you can still override them and use mouse bools pressed instead from the engine class 
extern "C" VOODOOENGINE_API void BroadcastInput(
	std::vector<InputCallback*> StoredCallbacks, int Input, bool Pressed);
//---------------------

// Utility
//---------------------
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
//---------------------

// Base component that all movable components inherit, sets the component location
//---------------------
class TransformComponent
{
public:
	SVector ComponentLocation = { 0, 0 };
};
//---------------------

// Bitmap
//---------------------
// Bitmap parameters, all the info about the bitmap
struct SBitmapParameters
{
	int RenderLayer = 0;
	float Opacity = 1;
	bool HiddenInGame = false;
	SVector BitmapOffsetLeft = { 0, 0 };
	SVector BitmapOffsetRight = { 0, 0 };
	SVector BitmapSource = { 0, 0 };
};
// Bitmap component, contains bitmap pointer, bitmap params and transform component
class BitmapComponent : public TransformComponent
{
public:
	ID2D1Bitmap* Bitmap = nullptr;
	SBitmapParameters BitmapParams = {};
};
// Set desired bitmap image, 
// if you want to flip an image e.g. for player/enemy movement
// call this function with the desired bitmap pointer as the return type 
// e.g. MyBitmapPointer = SetBitmap(RenderTarget, FileName, Flip)
extern "C" VOODOOENGINE_API ID2D1Bitmap* SetBitmap(
	ID2D1HwndRenderTarget* RenderTarget, const wchar_t* FileName, bool Flip = false);
// Setup bitmap struct
extern "C" VOODOOENGINE_API SBitmapParameters SetupBitmapParams(ID2D1Bitmap* CreatedBitmap);
// Set the bitmap source location on the bitmap image, use multiplier if you want to offset bitmap source X axis
extern "C" VOODOOENGINE_API void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int LocationOffsetMultiplier = 1);
// Set the bitmap source location on the bitmap image, use multiplier if you want to offset bitmap source Y axis
extern "C" VOODOOENGINE_API void SetBitmapSourceLocationY(
	BitmapComponent* BitmapToUpdate, int BitmapSourceHeight, int LocationOffsetMultiplier = 1);
//---------------------

// Animation
//---------------------
// Contains animation info such as frame size/total frames and animation speed/state info 
struct SAnimationParameters
{
	int AnimationState = 1;
	float AnimationSpeed = 1;
	int TotalFrames = 1;
	int FrameWidth = 0;
	int FrameHeight = 0;
	int CurrentFrame = 1;
	float AnimationTimer = 0;
};
// Set animation state e.g. idle, run
extern "C" VOODOOENGINE_API void SetAnimationState(
	SAnimationParameters& AnimationParams,
	SVector& BitmapSource,
	SVector& BitmapOffsetLeft,
	SVector& BitmapOffsetRight);
// Update animation
extern "C" VOODOOENGINE_API void UpdateAnimation(
	SAnimationParameters& AnimationParams,
	SVector& BitmapSource,
	SVector& BitmapOffsetLeft,
	SVector& BitmapOffsetRight,
	float DeltaTime);
// Setup the first frame of animation, 
// to use it, call this function inside "OnGameObjectSetupCompleted" virtual function in any gameobject 
// (used for when an object is created before activation of update component, 
// to make sure animation spritesheet bitmap gets framed with the first animation frame)
extern "C" VOODOOENGINE_API void InitializeAnimationFirstFrame(
	SAnimationParameters& AnimationParams, 
	SVector& BitmapSource, SVector& BitmapOffsetLeft, SVector& BitmapOffsetRight);
//---------------------

// Object
//---------------------
// Base object class inherited by all objects in the engine that will be transformed 
// and/or needs collision overlap events
//---------------------
class Object
{
public:
	SVector Location = { 0, 0 };

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
// Gets the result of the addition of the owner object (e.g. player) location, 
// and the component (e.g. gun) location 
extern "C" VOODOOENGINE_API SVector GetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component);
//---------------------

// Collision
//---------------------
// Collision component class, 
// contains all the needed collision parameters, 
// inherited by all objects that needs collision detection
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
	Object* Owner = nullptr;
};
extern "C" VOODOOENGINE_API bool IsCollisionDetected(
	CollisionComponent* Sender, CollisionComponent* Target);
extern "C" VOODOOENGINE_API void BroadcastCollision(
	Object* CallbackOwner, CollisionComponent* Sender, CollisionComponent* Target);
//---------------------

// Rendering
//---------------------
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
//---------------------

// Window
//---------------------
// Window parameters information i.e. title name of application, window resolution, fullscreen etc.  
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
extern "C" VOODOOENGINE_API void SetCustomAppIcon(LPCWSTR IconFileName, HWND& HWind);
// Updates any changes made to the app window (e.g. dragging the window)
extern "C" VOODOOENGINE_API void UpdateAppWindow();
//---------------------

// Update component inherited by all objects that needs to update each frame
//---------------------
class UpdateComponent
{
public:
	virtual void Update(float DeltaTime) = 0;
};
//---------------------

// Mouse class contains all that is needed for mouse support in the engine
//---------------------
class VoodooMouse : public Object
{
public:
	BitmapComponent MouseBitmap;
	CollisionComponent MouseCollider;
	Object* MouseHoveredObject = nullptr;
	bool PrimaryMousePressed = false;
	bool SecondaryMousePressed = false;
};
//---------------------

// Button
//---------------------
// Button state enum 
// (used by e.g. "UpdateButtonState" function)
enum EButtonState
{
	Default,
	Disabled,
	Hidden
};
// Button type enum
enum EButtonType
{
	TwoSided,
	OneSided,
	AssetButtonThumbnail
};
// Contains all the information for buttons 
struct SButtonParameters
{
	EButtonType ButtonType = TwoSided;
	const wchar_t* AssetPathButtonBitmap = L"";
	int ButtonCollisionTag = 0;
	std::string ButtonTextString = "";
	SVector ButtonTextOffset = { -2, 10 };
	SVector ButtonLocation = { 0, 0 };
};
// Generic button class (used by level editor, can be used for game UI)
class Button
{
public:
	CollisionComponent ButtonCollider;
	BitmapComponent ButtonBitmap;
	BitmapComponent AdditionalBackgroundBitmap;
	SButtonParameters ButtonParams = {};
	std::vector<BitmapComponent*> ButtonText;
};

// GameState callback
//---------------------
// Sends optionial broadcast to inherited object listeners when game has started/ended
class GameStateCallback
{
public:
	// Optional to setup for each game object
	virtual void OnGameStart(){};
	virtual void OnGameEnd(){};
};
//---------------------

// Game object 
//---------------------
// This class is used for all objects placed in levels, 
// (if a game object needs custom stuff e.g. object with more than a single bitmap/collision etc. 
// then this class would be a parent class to that object)
class GameObject : public Object, public GameStateCallback
{
public:
	// Optional custom constructor, called after everything has been initialized for the game object
	virtual void OnGameObjectCreated(SVector SpawnLocation){};

	// Optional custom deconstructor, called right before an object is set to be deleted, 
	// can be used to delete additional stuff created outside of this class in their own class
	// e.g. a player class where additional bitmaps/collision has been created inside player class 
	// that needs to be deleted
	virtual void OnGameObjectDeleted(){};

	BitmapComponent GameObjectBitmap;
	int GameObjectNumberID = 0;

	// Optional, can be set to not be created
	CollisionComponent AssetCollision;
	bool CreateDefaultAssetCollisionInGame = false;
};
//---------------------

// Asset related
//--------------------- 
// Asset struct
// This is used by level editor and save/load system 
// for the assets built in the levels in the game
struct SAssetParameters
{
	const wchar_t* AssetPath = L"";
	bool CreateDefaultAssetCollision = false;
};
// Asset button struct (used by level editor)
// PLEASE NOTE: Only numbers as "0" and up can be accounted for as ID numbers for assets 
// (since the level editor use negative numbers as ID)
struct SAssetButton
{
	Button* AssetButton = nullptr;
	int AssetID = 0;
	SAssetParameters AssetParams;

	bool operator==(const SAssetButton& Other) const
	{
		return (AssetButton == Other.AssetButton);
	}
};
//---------------------

// Engine class
class VoodooEngine
{
public:
	VoodooMouse Mouse;
	bool DebugMode = false;
	bool EditorMode = false;
	bool EngineRunning = false;
	bool GameRunning = false;
	SWindowParams Window;
	ID2D1HwndRenderTarget* Renderer = nullptr;
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER EndCounter;
	LARGE_INTEGER Counts;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER FPS;
	float TargetSecondsPerFrame = 1 / 100;
	float DeltaTime = 0;
	void(*AssetLoadFunctionPointer)(int, SVector);
	std::vector<UpdateComponent*> StoredUpdateComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<BitmapComponent*> StoredBitmapComponents;
	std::map<int, bool> StoredInputs;
	std::vector<InputCallback*> StoredInputCallbacks;
	std::vector<GameStateCallback*> StoredGameStateCallbacks;
	std::vector<GameObject*> StoredGameObjects;
	std::map<int, SAssetParameters> StoredGameObjectIDs;
	
	// This determines the letter space for any texts created
	int LetterSpace = 12;

	// Used to store buttons information 
	// (can be used for both in game UI and editor UI buttons)
	std::vector<BitmapComponent*> StoredButtonBitmapComponents;
	std::vector<BitmapComponent*> StoredButtonTexts;
	std::vector<CollisionComponent*> StoredEditorCollisionComponents;

	// Default collision rect color for editor mode assets
	SColor EditorCollisionRectColor = { 200, 0, 255 };

	// Only used in editor mode
	SVector AssetButtonThumbnailDimensions = { 90, 90 };
	std::vector<UpdateComponent*> StoredEditorUpdateComponents;
	std::vector<BitmapComponent*> StoredEditorBitmapComponents;
	std::vector<SAssetButton> StoredButtonAssets;
	// Used only for screen debug print
	std::vector<BitmapComponent*> StoredScreenPrintTexts;
	// This keeps track of the number of console text prints have been printed
	// (Offsets a newly printed text down a column if text in column already has been printed)
	// Will reset once console window is deleted
	int ScreenPrintTextColumnsPrinted = 0;

	void StartGame()
	{
		GameRunning = true;
		for (int i = 0; i < StoredGameObjects.size(); i++)
		{
			StoredGameObjects[i]->OnGameStart();
		}
	}
	void EndGame()
	{
		GameRunning = false;
		for (int i = 0; i < StoredGameObjects.size(); i++)
		{
			StoredGameObjects[i]->OnGameEnd();
		}
	}

	// Creates an instance game object based on asset ID
	// If no valid ID is found, then no object will be created with early return
	template<class T>
	void CreateGameObject(T* ClassToSpawn, int AssetID, SVector SpawnLocation)
	{
		auto Iterator = StoredGameObjectIDs.find(AssetID);

		// If object id is not found, invalidate and early return
		if (Iterator == StoredGameObjectIDs.end())
		{
			return;
		}

		const wchar_t* AssetPath = Iterator->second.AssetPath;
		bool CreateAssetCollision = Iterator->second.CreateDefaultAssetCollision;

		StoredGameObjects.push_back(new T);
		StoredGameObjects.back()->Location = SpawnLocation;
		StoredGameObjects.back()->GameObjectNumberID = AssetID;
		StoredGameObjects.back()->CreateDefaultAssetCollisionInGame = CreateAssetCollision;
		StoredGameObjects.back()->GameObjectBitmap.Bitmap = SetBitmap(Renderer, AssetPath);
		StoredGameObjects.back()->GameObjectBitmap.BitmapParams =
			SetupBitmapParams(StoredGameObjects.back()->GameObjectBitmap.Bitmap);
		StoredGameObjects.back()->GameObjectBitmap.ComponentLocation = SpawnLocation;
		StoredBitmapComponents.push_back(&StoredGameObjects.back()->GameObjectBitmap);

		// If in editor mode, create a clickable collision rect for the spawned game object 
		// in order for it to be selectable in the level editor
		if (EditorMode || CreateAssetCollision)
		{
			SVector GameObjectBitmapSize = 
				{ StoredGameObjects.back()->GameObjectBitmap.Bitmap->GetSize().width,
				StoredGameObjects.back()->GameObjectBitmap.Bitmap->GetSize().height };
			StoredGameObjects.back()->AssetCollision.CollisionRect = GameObjectBitmapSize;
			StoredGameObjects.back()->AssetCollision.ComponentLocation = SpawnLocation;
			StoredGameObjects.back()->AssetCollision.CollisionTag = AssetID;
			StoredGameObjects.back()->AssetCollision.Owner = StoredGameObjects.back();
			// Only set to render collision rect if in debug mode
			if (DebugMode)
			{
				StoredGameObjects.back()->AssetCollision.RenderCollisionRect = true;
				StoredGameObjects.back()->AssetCollision.CollisionRectColor = EditorCollisionRectColor;
			}
			StoredCollisionComponents.push_back(&StoredGameObjects.back()->AssetCollision);
		}
		StoredGameObjects.back()->OnGameObjectCreated(SpawnLocation);
	};
	// Removes game object from memory
	// Before it gets deleted a custom deconstructor "OnGameObjectDeleted"
	// (virtual function, not pure but optional) is called
	// This can be used to delete additional stuff created within the class
	template <class T> 
	void DeleteGameObject(T* ClassToDelete)
	{
		RemoveBitmapComponent(&ClassToDelete->GameObjectBitmap, this);
		if (ClassToDelete->GameObjectBitmap.Bitmap != nullptr)
		{
			ClassToDelete->GameObjectBitmap.Bitmap->Release();
			ClassToDelete->GameObjectBitmap.Bitmap = nullptr;
		}
		RemoveCollisionComponent(&ClassToDelete->AssetCollision, this);
		
		StoredGameObjects.erase(std::remove(
			StoredGameObjects.begin(),
			StoredGameObjects.end(), ClassToDelete));

		// Custom optional deconstructor called before delete
		// in case for example you want to delete anything custom made
		// created outside of the default game object base class stuff 
		// such as "GameObjectBitmap", "AssetCollision" etc.
		ClassToDelete->OnGameObjectDeleted();
		delete ClassToDelete;
		ClassToDelete = nullptr;
	};
	void DeleteAllGameObjects()
	{
		while (!StoredGameObjects.empty())
		{
			for (int i = 0; i < StoredGameObjects.size(); i++)
			{
				if (StoredGameObjects[i] != nullptr)
				{
					DeleteGameObject(StoredGameObjects[i]);
				}
			}
		}

		std::vector<BitmapComponent*>().swap(StoredBitmapComponents);
		std::vector<CollisionComponent*>().swap(StoredCollisionComponents);
		std::vector<GameObject*>().swap(StoredGameObjects);
	};
};
extern "C" VOODOOENGINE_API void UpdateMouseInput(VoodooEngine* Engine, UINT Message);
extern "C" VOODOOENGINE_API void CreateMouse(VoodooEngine* Engine, SVector MouseColliderSize);
extern "C" VOODOOENGINE_API void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize);
extern "C" VOODOOENGINE_API void UpdateMouseLocation(VoodooEngine* Engine, SVector NewLocation);
extern "C" VOODOOENGINE_API void UpdateCustomMouseCursor(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API bool HideSystemMouseCursor(UINT Message, LPARAM LParam);
extern "C" VOODOOENGINE_API void RenderCustomMouseCursor(
	ID2D1HwndRenderTarget* Renderer, VoodooEngine* Engine);
extern "C" VOODOOENGINE_API Button* CreateButton(
	VoodooEngine* Engine, 
	Button* ButtonToCreate, 
	int ButtonID, 
	EButtonType ButtonType,
	std::string ButtonText,
	SVector ButtonLocation, const wchar_t* AssetPath);

extern "C" VOODOOENGINE_API Button* DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete);
extern "C" VOODOOENGINE_API void UpdateButtonState(
	VoodooEngine* Engine, Button* ButtonToUpdate, EButtonState NewButtonState);
//-------------------------------------------

// Editor asset list
struct SEditorAssetList
{
	// Engine
	const wchar_t* GameIcon = L"EngineContent/Ico/GameIcon.ico";
	const wchar_t* EditorIcon = L"EngineContent/Ico/EditorIcon.ico";

	// Font asset paths
	const wchar_t* DefaultFont = L"EngineContent/Font/FontMonogram.png";
	const wchar_t* DebugFont = L"EngineContent/Font/FontMonogramDebug.png";

	// Used for debug only 
	const wchar_t* AnimationSpritesDebug = L"EngineContent/Debug/AnimationTesting.png";

	// Level editor
	//-----------------
	const wchar_t* LevelEditorUI = L"EngineContent/LevelEditor/LevelEditorUI.png";
	const wchar_t* LevelEditorTileIconBase = L"EngineContent/LevelEditor/TileIconBase.png";
	// Buttons
	const wchar_t* LevelEditorButtonDefaultW140 = 
		L"EngineContent/LevelEditor/DefaultButtonW140.png";
	const wchar_t* LevelEditorButtonActivateDeactivateW140 = 
		L"EngineContent/LevelEditor/ActivateDeactivateButtonW140.png";
	// Gizmo
	const wchar_t* Gizmo = L"EngineContent/LevelEditor/Gizmo.png";
	//-----------------
};

//---------------------
// Remove bitmap component from "StoredBitmapComponents" 
extern "C" VOODOOENGINE_API void RemoveBitmapComponent(BitmapComponent* Component, VoodooEngine* Engine);
// Remove collision component from "StoredCollisionComponents"
extern "C" VOODOOENGINE_API void RemoveCollisionComponent(CollisionComponent* Component, VoodooEngine* Engine);
// Remove update component from "StoredUpdateComponents"
extern "C" VOODOOENGINE_API void RemoveUpdateComponent(UpdateComponent* Component, VoodooEngine* Engine);
// Remove input callback component from "StoredInputCallbacks"
extern "C" VOODOOENGINE_API void RemoveInputCallback(InputCallback* Component, VoodooEngine* Engine);

// Print debug text to screen
extern "C" VOODOOENGINE_API void ScreenPrint(std::string DebugText, VoodooEngine* Engine);
// Clear all debug text from screen
extern "C" VOODOOENGINE_API void ClearScreenPrint(VoodooEngine* Engine);
// Called when user has quit the application
extern "C" VOODOOENGINE_API void CloseApp(VoodooEngine* Engine);
//---------------------

// Gizmo
//---------------------
class Gizmo : public Object, public UpdateComponent, public InputCallback
{
public:
	VoodooEngine* Engine = nullptr;
	BitmapComponent GizmoBitmap;
	CollisionComponent GizmoCollision;

	GameObject* SelectedGameObject = nullptr;
	GameObject* CurrentClickedGameObject = nullptr;
	bool GameObjectMouseHover = false;
	bool GizmoMouseHover = false;
	bool CanDragGizmo = false;
	SVector MouseClickLocationOffset;
	int GizmoSnapSize = 10;
	int GizmoCollisionRectSize = 70;
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

	void InitGizmo(VoodooEngine* EngineReference)
	{
		Engine = EngineReference;
		SEditorAssetList Asset;
		GizmoBitmap.Bitmap = SetBitmap(Engine->Renderer, Asset.Gizmo);
		GizmoBitmap.BitmapParams = SetupBitmapParams(GizmoBitmap.Bitmap);
		RenderGizmoCollisionRect = Engine->DebugMode;
		SetupGizmoCollisionTag();
		SetupGizmoCollisionRect();
		Engine->StoredEditorUpdateComponents.push_back(this);
		Engine->StoredEditorBitmapComponents.push_back(&GizmoBitmap);
		Engine->StoredEditorCollisionComponents.push_back(&GizmoCollision);
		Engine->StoredInputCallbacks.push_back(this);
	};

	void InitGizmoLocation(SVector NewLocation)
	{
		Location = NewLocation;
	};

	void SetGizmoState(bool Hidden)
	{
		if (Hidden)
		{
			GizmoBitmap.BitmapParams.HiddenInGame = true;
			GizmoCollision.NoCollision = true;
		}
		else
		{
			GizmoBitmap.BitmapParams.HiddenInGame = false;
			GizmoCollision.NoCollision = false;
		}
	};

	void SetMouseClickGizmoLocationOffset()
	{
		MouseClickLocationOffset.X = Location.X - Engine->Mouse.Location.X;
		MouseClickLocationOffset.Y = Location.Y - Engine->Mouse.Location.Y;
	};

	void UpdateMouseDragSnapLocationGizmo()
	{
		if (Engine->Mouse.PrimaryMousePressed &&
			CanDragGizmo)
		{
			Location.X =
				(int)((Engine->Mouse.Location.X +
				MouseClickLocationOffset.X) / GizmoSnapSize) * GizmoSnapSize;
			Location.Y =
				(int)((Engine->Mouse.Location.Y +
				MouseClickLocationOffset.Y) / GizmoSnapSize) * GizmoSnapSize;
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
		if (Engine->Mouse.PrimaryMousePressed &&
			CanDragGizmo && 
			SelectedGameObject != nullptr)
		{
			SVector NewLocation = 
				{ Location.X - GetGizmoOffsetLocation().X,
				Location.Y - GetGizmoOffsetLocation().Y };

			SelectedGameObject->Location = NewLocation;
			SelectedGameObject->GameObjectBitmap.ComponentLocation = NewLocation;
			SelectedGameObject->AssetCollision.ComponentLocation = NewLocation;
		}
	};

	bool IsMouseHoveringGizmo()
	{
		if (IsCollisionDetected(&GizmoCollision, &Engine->Mouse.MouseCollider))
		{
			return true;
		}
		
		return false;
	};
	
	bool IsMouseHoveringGameObject()
	{
		bool CollisionDetected = false;
		for (int i = 0; i < Engine->StoredCollisionComponents.size(); i++)
		{
			BroadcastCollision(this, &Engine->Mouse.MouseCollider, Engine->StoredCollisionComponents[i]);
			Engine->Mouse.MouseCollider.IsOverlapped = false;

			if (IsCollisionDetected(&Engine->Mouse.MouseCollider, Engine->StoredCollisionComponents[i]))
			{
				CollisionDetected = true;
			}
		}

		return CollisionDetected;
	};

	void Update(float DeltaTime)
	{
		if (Engine->GameRunning)
		{
			return;
		}

		GizmoMouseHover = IsMouseHoveringGizmo();
		GameObjectMouseHover = IsMouseHoveringGameObject();
		UpdateMouseDragSnapLocationGizmo();
		UpdateGizmoLocation();
		UpdateSelectedGameObjectDragLocation();
	};

	SVector GetGizmoOffsetLocation()
	{
		SVector OffsetLocation = 
		{ (SelectedGameObject->GameObjectBitmap.BitmapParams.BitmapSource.X / 2) - 10,
			(SelectedGameObject->GameObjectBitmap.BitmapParams.BitmapSource.Y / 2) -
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
		if (Engine->Mouse.MouseHoveredObject != nullptr)
		{
			SelectedGameObject = (GameObject*)(Engine->Mouse.MouseHoveredObject);
		}
	};

	void FullGizmoReset()
	{
		CanDragGizmo = false;
		SetGizmoState(true);
		Engine->Mouse.MouseHoveredObject = nullptr;
		SelectedGameObject = nullptr;
	};

	void InputBroadcast(int Input, bool Pressed)
	{
		if (Engine->GameRunning)
		{
			return;
		}

		if (Engine->Mouse.PrimaryMousePressed)
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

			CurrentClickedGameObject = nullptr;
			if (GameObjectMouseHover)
			{
				CurrentClickedGameObject = (GameObject*)(Engine->Mouse.MouseHoveredObject);
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

	void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag, Object* Target)
	{
		if (GameObjectMouseHover)
		{
			Engine->Mouse.MouseHoveredObject = Target;
		}
	};
};
//---------------------

// Level Editor
//-------------------------------------------
class LevelEditor : public Object, public UpdateComponent, public InputCallback
{

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

// Asset location spawned when asset button is clicked
#define ASSET_SELECTION_SPAWN_LOCATION_X 850
#define ASSET_SELECTION_SPAWN_LOCATION_Y 400

public:
	LevelEditor(VoodooEngine* EngineReference)
	{
		Engine = EngineReference;

		// Add level editor update function to be called every frame
		Engine->StoredEditorUpdateComponents.push_back(this);

		// Add input callback for level editor
		Engine->StoredInputCallbacks.push_back(this);

		// Create level editor UI
		LevelEditorUI.Bitmap = SetBitmap(Engine->Renderer, Asset.LevelEditorUI);
		LevelEditorUI.BitmapParams = SetupBitmapParams(LevelEditorUI.Bitmap);
		Engine->StoredEditorBitmapComponents.push_back(&LevelEditorUI);

		// Create all the clickable level editor buttons
		CreatePlayLevelButton();
		PreviousButton = CreateButton(
			Engine, PreviousButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS, 
			TwoSided, "previous", { BUTTON_LOC_X_PREVIOUS, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonDefaultW140);
		NextButton = CreateButton(
			Engine, NextButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT, 
			TwoSided,  "next", { BUTTON_LOC_X_NEXT, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonDefaultW140);

		// Create all clickable assets buttons
		CreateAssetButtons();
		// Display default asssets
		DisplayAssets(0);

		LevelEditorVisible = true;
	}

	Gizmo TransformGizmo;
	BitmapComponent LevelEditorUI;
	void(*ButtonPressedCallback)(int);
	bool LevelEditorVisible = false;

	void CreateGizmo()
	{
		TransformGizmo.InitGizmo(Engine);
		TransformGizmo.InitGizmoLocation({ 1000, 500 });
		TransformGizmo.SetGizmoState(true);
	};

	void CreatePlayLevelButton()
	{
		PlayLevelButton = CreateButton(
			Engine, PlayLevelButton, TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL, 
			TwoSided, "play_level", { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonDefaultW140);
	};
	void CreateStopPlayButton()
	{
		StopPlayButton = CreateButton(
			Engine, StopPlayButton, TAG_LEVEL_EDITOR_BUTTON_STOPPLAY, 
			TwoSided, "stop_play", { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonActivateDeactivateW140);
		
		SetBitmapSourceLocationX(
			&StopPlayButton->ButtonBitmap, 
			StopPlayButton->ButtonBitmap.Bitmap->GetSize().width / 2, 2);
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
			AssetButton = CreateButton(
				Engine, AssetButton, CurrentAssetID, AssetButtonThumbnail, "",
				{ ASSET_SELECTION_BUTTON_LOC_X_ORIGIN + LocXOffset,
				(ASSET_SELECTION_BUTTON_LOC_Y_ORIGIN + LocYOffset) },
				CurrentStoredButtonAssets[i].AssetParams.AssetPath);
			
			CurrentStoredButtonAssets[i].AssetButton = AssetButton;
			LocYOffset += OffsetYAmount;

			Index++;
		}
	};
	void DeletePlayLevelButton()
	{
		PlayLevelButton = DeleteButton(Engine, PlayLevelButton);
		HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
	};
	void DeleteStopPlayButton()
	{
		StopPlayButton = DeleteButton(Engine, StopPlayButton);
		HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
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
					Engine->StoredButtonBitmapComponents.end(), &AssetToDelete.AssetButton->ButtonBitmap));
				
				Engine->StoredEditorCollisionComponents.erase(std::remove(
					Engine->StoredEditorCollisionComponents.begin(),
					Engine->StoredEditorCollisionComponents.end(), &AssetToDelete.AssetButton->ButtonCollider));

				delete AssetToDelete.AssetButton;
			}
		}

		CurrentStoredButtonAssets.clear();

		if (CurrentStoredButtonAssets.empty())
		{
			//ScreenPrint("assets_empty", Engine);
		}
	};
	void ResetButtonsBitmapSource(Button* ButtonBitmapToReset)
	{
		if (!ButtonBitmapToReset)
		{
			return;
		}

		// Reset to the default bitmap source location (if two sided e.g. pressed/not pressed)
		SetBitmapSourceLocationX(
			&ButtonBitmapToReset->ButtonBitmap,
			ButtonBitmapToReset->ButtonBitmap.Bitmap->GetSize().width / 2);
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
			&ButtonBitmapToSet->ButtonBitmap,
			ButtonBitmapToSet->ButtonBitmap.Bitmap->GetSize().width / 2,
			ButtonClickedSourceLocationOffsetX);
	};
	void UpdateAssetThumbnailButtonsState(EButtonState NewButtonState)
	{
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
		{
			UpdateButtonState(Engine, CurrentStoredButtonAssets[i].AssetButton, NewButtonState);
		}
	};
	void UpdateAllButtonsState(EButtonState NewButtonState)
	{
		UpdateButtonState(Engine, PlayLevelButton, NewButtonState);
		UpdateButtonState(Engine, StopPlayButton, NewButtonState);
		UpdateButtonState(Engine, PreviousButton, NewButtonState);
		UpdateButtonState(Engine, NextButton, NewButtonState);
		UpdateAssetThumbnailButtonsState(NewButtonState);
	};
	void UpdateLevelEditorVisibility(bool Hide)
	{
		if (Hide)
		{
			HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
			LevelEditorVisible = false;
			LevelEditorUI.BitmapParams.HiddenInGame = true;
			UpdateAllButtonsState(EButtonState::Hidden);
		}
		else if (!Hide)
		{
			LevelEditorVisible = true;
			LevelEditorUI.BitmapParams.HiddenInGame = false;
			UpdateAllButtonsState(EButtonState::Default);
		}
	};
	void OnButtonPressed()
	{
		ButtonPressedCallback(HoveredButtonID);

		switch (HoveredButtonID)
		{
		case TAG_LEVEL_EDITOR_BUTTON_SAVELEVEL:
			break;
		case TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL:
			break;
		case TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL:
			DeletePlayLevelButton();
			CreateStopPlayButton();
			Engine->StartGame();
			TransformGizmo.SetGizmoState(true);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_STOPPLAY:
			DeleteStopPlayButton();
			CreatePlayLevelButton();
			Engine->EndGame();
			TransformGizmo.FullGizmoReset();
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS:
			SetButtonBitmapSourceClicked(PreviousButton);
			DisplayAssets(TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT:
			SetButtonBitmapSourceClicked(NextButton);
			DisplayAssets(TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT);
			break;
		default:
			Engine->AssetLoadFunctionPointer(HoveredButtonID, 
				{ ASSET_SELECTION_SPAWN_LOCATION_X, ASSET_SELECTION_SPAWN_LOCATION_Y });
			break;
		}
	};

	void InputBroadcast(int Input, bool Pressed)
	{
		if (Engine->Mouse.PrimaryMousePressed)
		{
			OnButtonPressed();
		}
		else if (!Engine->Mouse.PrimaryMousePressed)
		{
			ResetButtonsBitmapSource(PlayLevelButton);
			ResetButtonsBitmapSource(PreviousButton);
			ResetButtonsBitmapSource(NextButton);
		}

		if (Input == VK_DELETE &&
			TransformGizmo.SelectedGameObject != nullptr)
		{
			Engine->DeleteGameObject(TransformGizmo.SelectedGameObject);
			TransformGizmo.SelectedGameObject = nullptr;
			TransformGizmo.SetGizmoState(true);
		}

		if (Input == VK_TAB)
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
	};
	void UpdateButtonCollisionCheck(Button* ButtonToUpdate)
	{
		if (ButtonToUpdate == nullptr)
		{
			return;
		}

		BroadcastCollision(this, &ButtonToUpdate->ButtonCollider, &Engine->Mouse.MouseCollider);
	};
	void Update(float DeltaTime)
	{
		if (Engine == nullptr)
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

			BroadcastCollision(
				this, &CurrentStoredButtonAssets[i].AssetButton->ButtonCollider,
				&Engine->Mouse.MouseCollider);
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
	VoodooEngine* Engine = nullptr;
	int HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
	SEditorAssetList Asset;
	SAssetIndex AssetIndexDisplayed;
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
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
		{
			CurrentStoredButtonAssets[i].AssetButton->ButtonBitmap.BitmapParams.HiddenInGame = true;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.NoCollision = true;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.RenderCollisionRect = false;
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

			CurrentStoredButtonAssets[i].AssetButton->ButtonBitmap.BitmapParams.HiddenInGame = false;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.NoCollision = false;
			CurrentStoredButtonAssets[i].AssetButton->ButtonCollider.RenderCollisionRect = true;
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
extern "C" VOODOOENGINE_API bool SetDebugMode();
extern "C" VOODOOENGINE_API bool SetEditorMode();
// Read/Write (saving/loading)
extern "C" VOODOOENGINE_API void LoadGameObjectsFromFile(
	char* FileName, VoodooEngine* Engine);
//-------------------------------------------
