#pragma once

// VoodooEngine is a complete engine framework for making a 2D game
// --------------------
// Supported features:
// 
// Graphics
// - Creation of an application window with custom title name and icon using "Win32 API"
// - Bitmap rendering using "Direct2D API"
// - Spritesheet animation
// - Dynamic flip of bitmap at runtime
// - Text rendering using "DirectWrite API"
// 
// Gameplay
// - User input for keyboard (using "Win32 API") and gamepad (using "XInput API") 
// - Collision detection using "AABB" algorithm
// - Playing one shot sounds/looping sounds using "XAudio2 API"
// - Update function with deltatime
// - Class/Interface support for gameplay such as, 
// creating gameobjects dynamically during gameplay using base gameobject class 
// and sending interface message to gameobjects e.g. open door, take damage from enemy etc.
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
// - "I" stands for interface e.g. IInteract, ITakeDamage
// - Every function name with prefix "Update" e.g. "UpdateAnimation", will be called every frame

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
#include <d2d1_3.h>
#pragma comment(lib, "d2d1.lib")

// DirectWrite API
#include <dwrite.h>
#pragma comment(lib, "Dwrite.lib")

#define VOODOOENGINE_API __declspec(dllexport)

// Maximum number of allowed renderlayers
#define RENDERLAYER_MAXNUM 10

// Render layer number IDs
#define RENDERLAYER_0 0
#define RENDERLAYER_1 1
#define RENDERLAYER_2 2
#define RENDERLAYER_3 3
#define RENDERLAYER_4 4
#define RENDERLAYER_5 5
#define RENDERLAYER_6 6
#define RENDERLAYER_7 7
#define RENDERLAYER_8 8
#define RENDERLAYER_9 9
#define RENDERLAYER_10 10

// Global collision tags used by the editor
#define TAG_LEVEL_EDITOR_GIZMO -1
#define TAG_LEVEL_EDITOR_BUTTON_ID_NONE -2
#define TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL -3
#define TAG_LEVEL_EDITOR_BUTTON_SAVELEVEL -4
#define TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL -5
#define TAG_LEVEL_EDITOR_BUTTON_STOPPLAY -6
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS -7
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT -8
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_ASSETS -9
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_RENDERLAYERS -10
#define TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_VIEWMODE -11

// Window parameters information i.e. title name of application, window resolution, fullscreen etc.  
struct SWindowParams
{
	HWND HWind = nullptr;
	WNDCLASSEX WindowClass;
	LPCWSTR WindowTitle;
	int ScreenResolutionWidth;
	int ScreenResolutionHeight;
	bool Fullscreen = true;
};

enum ETextBrushRenderType
{
	RenderWhiteBrush,
	RenderBlackBrush
};

// Struct used to determine the UI text parameters
//---------------------
struct UITextParameters
{
	const wchar_t* Text = L"";
	ETextBrushRenderType TextRenderType = ETextBrushRenderType::RenderWhiteBrush;
	bool HideText = false;
};
//---------------------

// Input
//---------------------
// Input interface with input type and if pressed/not pressed, input type ID is set by specific game
class IInput
{
public:
	virtual void OnInputBroadcast(int Input, bool Pressed){};
};
// Sends input broadcast to all listeners whenever an input is pressed, 
// input type ID is set by specific game
extern "C" VOODOOENGINE_API void BroadcastInput(
	std::vector<IInput*> StoredCallbacks, int Input, bool Pressed);
//---------------------

// Generic event interface with no parameters 
class IEvent
{
public:
	virtual void OnEventBroadcast(){};
};

// Interface used for when certain objects wants to handle custom render logic
class IRender
{
public:
	virtual void OnRenderBroadcast(ID2D1HwndRenderTarget* Renderer){};
};

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
	// Default white color
	float R = 1;
	float G = 1;
	float B = 1;
};
//---------------------

// Base component that all movable components inherit, sets the component location
//---------------------
class TransformComponent
{
public:
	SVector ComponentLocation;
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
	SVector BitmapOffsetLeft;
	SVector BitmapOffsetRight;
	SVector BitmapSource;
};
// Bitmap component, contains bitmap pointer, bitmap params and transform component
class BitmapComponent : public TransformComponent
{
public:
	ID2D1Bitmap* Bitmap = nullptr;
	SBitmapParameters BitmapParams = {};
};

// Flip a bitmap, can be used for e.g. for player/enemy movement 
// (if "Flip" is set to false then bitmap will be "un flipped") 
extern "C" VOODOOENGINE_API ID2D1Bitmap* FlipBitmap( 
	ID2D1Bitmap* BitmapToFlip,
	const wchar_t* FileName,
	ID2D1HwndRenderTarget* Renderer, bool Flip = true);

// Setup a bitmap
extern "C" VOODOOENGINE_API ID2D1Bitmap* SetupBitmap(
	ID2D1Bitmap* BitmapToSetup, const wchar_t* FileName, ID2D1HwndRenderTarget* Renderer);

// Setup bitmap component from a texture atlas (created bitmap)
extern "C" VOODOOENGINE_API void SetupBitmapComponent(
	BitmapComponent* BitmapComponentToSetup,
	ID2D1Bitmap* TextureAtlas,
	SVector TextureAtlasWidthHeight = {},
	SVector TextureAtlasOffsetMultiplierWidthHeight = {},
	bool UseEntireTextureAtlasAsBitmapSource = true);

// Set the bitmap source location on the bitmap image, 
// use multiplier if you want to offset bitmap source X axis
extern "C" VOODOOENGINE_API void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int LocationOffsetMultiplier = 1);

// Set the bitmap source location on the bitmap image, 
// use multiplier if you want to offset bitmap source Y axis
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
extern "C" VOODOOENGINE_API void InitAnimationFirstFrame(
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
	SVector Location;

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
// and the component (e.g. player's gun) location 
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
	bool DrawFilledRectangle = false;
	float Opacity = 1;
	int CollisionTag = 0;
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
//---------------------

// Rendering
//---------------------
void RenderBitmapByLayer(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> StoredBitmaps, int RenderLayer);
void RenderBitmaps(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> BitmapsToRender, 
	int MaxNumRenderLayers = 0);
void RenderCollisionRectangles(
	ID2D1HwndRenderTarget* Renderer, std::vector<CollisionComponent*> CollisionRectsToRender);
void RenderCollisionRectangle(
	ID2D1HwndRenderTarget* Renderer, CollisionComponent* CollisionRectToRender);
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
// (used by e.g. "SetButtonState" function)
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
	SVector ButtonLocation;
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

	// Never account for negative value as game object ID as that is the default value
	int GameObjectID = -1;

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
	ID2D1Bitmap* TextureAtlas = nullptr;
	SVector TextureAtlasWidthHeight = { 0, 0 };
	SVector TextureAtlasOffsetMultiplierWidthHeight = { 1, 1 };
	int RenderLayer = 0;
	bool CreateDefaultAssetCollision = false;
	// Default to asset button empty thumbnail, can be overriden
	const wchar_t* EditorAssetButtonThumbnailFilePath = L"EngineContent/LevelEditor/AssetButtonBase.png";
};
// Asset button struct (used by level editor)
// PLEASE NOTE: Only numbers as "0" and up can be accounted for as ID numbers for assets 
// (since the level editor use negative numbers as ID for internal level editor stuff)
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
struct SWindowsProcedureParameters
{
	HWND HWind;
	UINT Message;
	WPARAM WParam;
	LPARAM LParam;
};

// Engine class
class VoodooEngine
{
public:
	inline static VoodooEngine* Engine = nullptr;
	VoodooMouse Mouse;
	bool DebugMode = false;
	bool EditorMode = false;
	bool EngineRunning = false;
	bool GameRunning = false;
	SWindowParams Window;
	ID2D1HwndRenderTarget* Renderer = nullptr;
	D2D1_COLOR_F ClearScreenColor = { 0, 0, 0 };

	// Level editor gizmo
	int LevelEditorGizmoSnapSize = 10;

	// Resolution of the screen
	int ScreenWidthDefault = 1920;
	int ScreenHeightDefault = 1080;

	// Frame rate related
	LARGE_INTEGER StartTicks;
	LARGE_INTEGER TicksPerSecond;
	LARGE_INTEGER CurrentTicks;
	int FPS = 100;
	int FrameTargetTime = (1000 / FPS);
	int PreviousFrameTime = 0;
	int TimeToWait = 0;
	float DeltaTime = 0;

	SWindowsProcedureParameters WinProcParams;

	// File I/O related
	char FileName[100];
	void(*AssetLoadFunctionPointer)(int, SVector);

	std::vector<IRender*> InterfaceObjects_Render;
	std::vector<IInput*> InterfaceObjects_InputCallback;

	// Game background
	BitmapComponent* CurrentGameBackground = nullptr;

	// The asset parameter struct contains variables in this order:
	// ID2D1Bitmap* TextureAtlas
	// SVector TextureAtlasWidthHeight
	// SVector TextureAtlasOffsetMultiplierWidthHeight
	// int RenderLayer
	// bool CreateDefaultAssetCollision
	// const wchar_t* EditorAssetButtonThumbnailFilePath
	std::map<int, SAssetParameters> StoredGameObjectIDs;
	
	// Stored game object related vectors
	std::vector<BitmapComponent*> StoredBitmapComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<GameObject*> StoredGameObjects;
	std::vector<UpdateComponent*> StoredUpdateComponents;

	// Used only for screen debug print
	std::vector<BitmapComponent*> StoredScreenPrintTexts;
	
	// This keeps track of the number of console text prints have been printed
	// (Offsets a newly printed text down a column if text in column already has been printed)
	// Will reset once console window is deleted
	int ScreenPrintTextColumnsPrinted = 0;

	// This determines the letter space for any texts created
	int LetterSpace = 12;

	// Only used in level editor mode 
	SVector AssetButtonThumbnailDimensions = { 90, 90 };
	std::vector<BitmapComponent*> StoredButtonTexts;
	std::vector<SAssetButton> StoredButtonAssets;
	std::vector<BitmapComponent*> StoredEditorBitmapComponents;
	std::vector<BitmapComponent*> StoredButtonBitmapComponents;
	std::vector<CollisionComponent*> StoredEditorCollisionComponents;
	std::vector<UpdateComponent*> StoredEditorUpdateComponents;

	// Default collision rect color for editor mode assets
	SColor EditorCollisionRectColor = { 200, 0, 255 };
	
	// Color options
	SColor ColorBlack = { 0, 0, 0 };
	SColor ColorWhite = { 255, 255, 255 };
	SColor ColorRed = { 255, 0, 0 };
	SColor ColorGreen = { 0, 255, 0 };
	SColor ColorBlue = { 0, 0, 255 };
	SColor ColorCyan = { 0, 255, 255 };
	SColor ColorYellow = { 255, 255, 0 };

	// Variables used by direct write to display UI text, 
	// they will be created and be available for the remainder of the program
	IDWriteTextFormat* TextFormat = nullptr;
	ID2D1SolidColorBrush* BlackBrush = nullptr;
	ID2D1SolidColorBrush* WhiteBrush = nullptr;
	// Used to store all UI text for render layers using directwrite
	std::map<int, UITextParameters> StoredRenderLayerUITexts;

	// Clear all debug text from screen
	static void ClearScreenPrint(VoodooEngine* Engine)
	{
		if (Engine->StoredScreenPrintTexts.empty())
		{
			return;
		}

		while (!Engine->StoredScreenPrintTexts.empty())
		{
			for (int i = 0; i < Engine->StoredScreenPrintTexts.size(); i++)
			{
				BitmapComponent* BitmapPointer = Engine->StoredScreenPrintTexts[i];
				Engine->StoredScreenPrintTexts.erase(std::remove(
					Engine->StoredScreenPrintTexts.begin(),
					Engine->StoredScreenPrintTexts.end(), BitmapPointer));
				BitmapPointer->Bitmap->Release();
				BitmapPointer->Bitmap = nullptr;
				delete BitmapPointer;
			}
		}

		Engine->ScreenPrintTextColumnsPrinted = 0;
	}

	static void CallMouseInputCallback(int MouseButton, bool Pressed)
	{
		for (int i = 0; i < VoodooEngine::Engine->InterfaceObjects_InputCallback.size(); i++)
		{
			VoodooEngine::Engine->InterfaceObjects_InputCallback[i]->OnInputBroadcast(MouseButton, Pressed);
		}
	}

	static void UpdateMouseInput(VoodooEngine* Engine, UINT Message)
	{
		switch (Message)
		{
			// Primary mouse button
		case WM_LBUTTONDOWN:
			Engine->Mouse.PrimaryMousePressed = true;
			CallMouseInputCallback(WM_LBUTTONDOWN, true);
			break;
		case WM_LBUTTONUP:
			Engine->Mouse.PrimaryMousePressed = false;
			CallMouseInputCallback(WM_LBUTTONUP, false);
			break;
			// Secondary mouse button
		case WM_RBUTTONDOWN:
			Engine->Mouse.SecondaryMousePressed = true;
			CallMouseInputCallback(WM_RBUTTONDOWN, true);
			break;
		case WM_RBUTTONUP:
			Engine->Mouse.SecondaryMousePressed = false;
			CallMouseInputCallback(WM_RBUTTONUP, false);
			break;
		}
	};

	static void UpdateKeyboardInput(VoodooEngine* Engine, UINT Message, WPARAM WParam)
	{
		switch (Message)
		{
		case WM_KEYDOWN:
			BroadcastInput(VoodooEngine::Engine->InterfaceObjects_InputCallback, WParam, true);
			break;
		case WM_KEYUP:
			BroadcastInput(VoodooEngine::Engine->InterfaceObjects_InputCallback, WParam, false);
			break;
		default:
			break;
		}
	}

	inline static LRESULT CALLBACK WindowProcedure(
		HWND HWind, UINT Message, WPARAM WParam, LPARAM LParam)
	{
		// Don't use engine if not running 
		if (!VoodooEngine::Engine->EngineRunning)
		{
			return DefWindowProc(HWind, Message, WParam, LParam);
		}

		// Stop running engine if pressed "X" icon in top right corner or escape key
		if (Message == WM_DESTROY || 
			Message == WM_KEYDOWN &&
			WParam == VK_ESCAPE)
		{
			VoodooEngine::Engine->EngineRunning = false;
		}

		// Clear screen print if visible (used for debugging)
		if (VoodooEngine::Engine->DebugMode &&
			Message == WM_KEYDOWN &&
			WParam == VK_DELETE)
		{
			ClearScreenPrint(VoodooEngine::Engine);
		}

		VoodooEngine::Engine->WinProcParams.HWind = HWind;
		VoodooEngine::Engine->WinProcParams.Message = Message;
		VoodooEngine::Engine->WinProcParams.WParam = WParam;
		VoodooEngine::Engine->WinProcParams.LParam = LParam;

		UpdateMouseInput(VoodooEngine::Engine, Message);
		UpdateKeyboardInput(VoodooEngine::Engine, Message, WParam);

		// Hides system mouse cursor since engine is using custom icon for cursor
		if (Message == WM_SETCURSOR &&
			LOWORD(LParam) == HTCLIENT)
		{
			SetCursor(NULL);
			return TRUE;
		}

		return DefWindowProc(HWind, Message, WParam, LParam);
	};

	void StartGame()
	{
		GameRunning = true;
		for (int i = 0; i < StoredGameObjects.size(); i++)
		{
			StoredGameObjects[i]->OnGameStart();
			if (!StoredGameObjects[i]->CreateDefaultAssetCollisionInGame)
			{
				StoredGameObjects[i]->AssetCollision.NoCollision = true;
			}
		}
	}
	void EndGame()
	{
		GameRunning = false;
		for (int i = 0; i < StoredGameObjects.size(); i++)
		{
			StoredGameObjects[i]->OnGameEnd();
			if (!StoredGameObjects[i]->CreateDefaultAssetCollisionInGame)
			{
				StoredGameObjects[i]->AssetCollision.NoCollision = false;
			}
		}
	}

	template<class T>
	void RemoveComponent(T* ObjectToRemove, std::vector<T*> *VectorToRemoveFrom)
	{
		VectorToRemoveFrom->erase(std::remove(
			VectorToRemoveFrom->begin(),
			VectorToRemoveFrom->end(), ObjectToRemove));
	};

	// Creates an instance game object based on class to spawn/asset ID
	// if no valid ID is found, then no object will be created and nullptr is returned
	// if valid ID the created object is returned
	template<class T>
	T* CreateGameObject(T* ClassToSpawn, int GameObjectID, SVector SpawnLocation)
	{
		auto Iterator = StoredGameObjectIDs.find(GameObjectID);

		// If object id is not found, invalidate and return nullptr
		if (Iterator == StoredGameObjectIDs.end())
		{
			ClassToSpawn = nullptr;
			return nullptr;
		}

		StoredGameObjects.push_back(new T);
		StoredGameObjects.back()->Location = SpawnLocation;
		StoredGameObjects.back()->GameObjectID = GameObjectID;
		StoredGameObjects.back()->CreateDefaultAssetCollisionInGame = 
			Iterator->second.CreateDefaultAssetCollision;
		SetupBitmapComponent(&StoredGameObjects.back()->GameObjectBitmap, 
			Iterator->second.TextureAtlas, 
			Iterator->second.TextureAtlasWidthHeight, 
			Iterator->second.TextureAtlasOffsetMultiplierWidthHeight, false);
		StoredGameObjects.back()->GameObjectBitmap.BitmapParams.RenderLayer = Iterator->second.RenderLayer;
		StoredGameObjects.back()->GameObjectBitmap.ComponentLocation = SpawnLocation;
		StoredBitmapComponents.push_back(&StoredGameObjects.back()->GameObjectBitmap);

		// If in editor mode, create a clickable collision rect for the spawned game object 
		// in order for it to be selectable in the level editor
		if (EditorMode || 
			Iterator->second.CreateDefaultAssetCollision)
		{
			StoredGameObjects.back()->AssetCollision.CollisionRect = 
				{ Iterator->second.TextureAtlasWidthHeight.X, Iterator->second.TextureAtlasWidthHeight.Y };
			StoredGameObjects.back()->AssetCollision.ComponentLocation = SpawnLocation;
			StoredGameObjects.back()->AssetCollision.CollisionTag = GameObjectID;
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
		return (T*)(StoredGameObjects.back());
	};
	
	// Removes game object from memory
	// before it gets deleted a custom deconstructor "OnGameObjectDeleted"
	// (virtual function, not pure but optional) is called
	// this can be used to delete additional stuff created within the class
	// returns nullptr
	template <class T> 
	T* DeleteGameObject(T* ClassToDelete)
	{		
		RemoveComponent(&ClassToDelete->GameObjectBitmap, &this->StoredBitmapComponents);
		RemoveComponent(ClassToDelete, &this->StoredGameObjects);

		if (EditorMode ||
			ClassToDelete->CreateDefaultAssetCollisionInGame)
		{
			RemoveComponent(&ClassToDelete->AssetCollision, &this->StoredCollisionComponents);
		}

		// Custom optional deconstructor called before delete
		// in case for example you want to delete anything custom made
		// created outside of the default game object base class stuff 
		// such as "GameObjectBitmap", "AssetCollision" etc.
		ClassToDelete->OnGameObjectDeleted();
		delete ClassToDelete;
		ClassToDelete = nullptr;
		return nullptr;
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

private:
	std::map<int, bool> StoredInputs;
};

// Used to setup check for collision detection on left, right, up and down sides of a game object 
struct SQuadCollisionParameters
{
	CollisionComponent CollisionLeft;
	CollisionComponent CollisionRight;
	CollisionComponent CollisionUp;
	CollisionComponent CollisionDown;

	SVector RectSizeCollisionLeft;
	SVector RectSizeCollisionRight;
	SVector RectSizeCollisionUp;
	SVector RectSizeCollisionDown;

	SVector RelativeOffsetCollisionLeft;
	SVector RelativeOffsetCollisionRight;
	SVector RelativeOffsetCollisionUp;
	SVector RelativeOffsetCollisionDown;

	bool CollisionHitLeft = false;
	bool CollisionHitRight = false;
	bool CollisionHitUp = false;
	bool CollisionHitDown = false;
};

class MovementComponent
{
public:	
	SVector MovementDirection;
	float MovementSpeed = 0;
	SQuadCollisionParameters QuadCollisionParams;
	bool IsJumping = false;
	bool IsFalling = false;

	void InitMovementComponent(
		SQuadCollisionParameters DesiredQuadCollisionParams, 
		float DesiredMovementSpeed, VoodooEngine* Engine)
	{
		InitCollisionRectangles(DesiredQuadCollisionParams, Engine);

		MovementSpeed = DesiredMovementSpeed;
	}

	void UpdateMovement(SVector NewLocation)
	{
		UpdateCollisionRectsLocation(NewLocation);
	}

	void RemoveMovementComponent(VoodooEngine* Engine)
	{
		Engine->RemoveComponent(
			&QuadCollisionParams.CollisionLeft, &Engine->StoredCollisionComponents);
		Engine->RemoveComponent(
			&QuadCollisionParams.CollisionRight, &Engine->StoredCollisionComponents);
		Engine->RemoveComponent(
			&QuadCollisionParams.CollisionUp, &Engine->StoredCollisionComponents);
		Engine->RemoveComponent(
			&QuadCollisionParams.CollisionDown, &Engine->StoredCollisionComponents);
	}

private:
	void InitCollisionRectangles(
		SQuadCollisionParameters DesiredQuadCollisionParams, VoodooEngine* Engine)
	{
		if (Engine->DebugMode)
		{
			QuadCollisionParams.CollisionLeft.RenderCollisionRect = true;
			QuadCollisionParams.CollisionRight.RenderCollisionRect = true;
			QuadCollisionParams.CollisionUp.RenderCollisionRect = true;
			QuadCollisionParams.CollisionDown.RenderCollisionRect = true;

			QuadCollisionParams.CollisionLeft.CollisionRectColor = Engine->ColorYellow;
			QuadCollisionParams.CollisionRight.CollisionRectColor = Engine->ColorYellow;
			QuadCollisionParams.CollisionUp.CollisionRectColor = Engine->ColorYellow;
			QuadCollisionParams.CollisionDown.CollisionRectColor = Engine->ColorYellow;
		}

		QuadCollisionParams.CollisionLeft.CollisionRect = 
			DesiredQuadCollisionParams.RectSizeCollisionLeft;
		QuadCollisionParams.CollisionRight.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionRight;
		QuadCollisionParams.CollisionUp.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionUp;
		QuadCollisionParams.CollisionDown.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionDown;

		QuadCollisionParams.RelativeOffsetCollisionLeft =
			DesiredQuadCollisionParams.RelativeOffsetCollisionLeft;
		QuadCollisionParams.RelativeOffsetCollisionRight =
			DesiredQuadCollisionParams.RelativeOffsetCollisionRight;
		QuadCollisionParams.RelativeOffsetCollisionUp =
			DesiredQuadCollisionParams.RelativeOffsetCollisionUp;
		QuadCollisionParams.RelativeOffsetCollisionDown =
			DesiredQuadCollisionParams.RelativeOffsetCollisionDown;

		Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionLeft);
		Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionRight);
		Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionUp);
		Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionDown);
	}

	void UpdateCollisionRectsLocation(SVector NewLocation)
	{
		QuadCollisionParams.CollisionLeft.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionLeft.X;
		QuadCollisionParams.CollisionLeft.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionLeft.Y;

		QuadCollisionParams.CollisionRight.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionRight.X;
		QuadCollisionParams.CollisionRight.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionRight.Y;

		QuadCollisionParams.CollisionUp.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionUp.X;
		QuadCollisionParams.CollisionUp.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionUp.Y;

		QuadCollisionParams.CollisionDown.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionDown.X;
		QuadCollisionParams.CollisionDown.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionDown.Y;
	}
};

// Custom mouse cursor related
//---------------------
extern "C" VOODOOENGINE_API void CreateMouse(VoodooEngine* Engine, SVector MouseColliderSize);
extern "C" VOODOOENGINE_API void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize);
extern "C" VOODOOENGINE_API void SetMouseState(bool Show, VoodooEngine* Engine);
//---------------------

extern "C" VOODOOENGINE_API Button* CreateButton(
	VoodooEngine* Engine, 
	Button* ButtonToCreate, 
	int ButtonID, 
	EButtonType ButtonType,
	std::string ButtonText,
	SVector ButtonLocation, const wchar_t* AssetPath);

extern "C" VOODOOENGINE_API Button* DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete);
extern "C" VOODOOENGINE_API void SetButtonState(
	Button* ButtonToUpdate, EButtonState NewButtonState, bool KeepBitmapOffsetUnchanged = false);
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
	const wchar_t* LevelEditorUITop = L"EngineContent/LevelEditor/LevelEditorUI_Top.png";
	const wchar_t* LevelEditorUIOverlay = L"EngineContent/LevelEditor/LevelEditorUI_Overlay.png";
	const wchar_t* LevelEditorTileIconBase = L"EngineContent/LevelEditor/TileIconBase.png";
	// Buttons
	const wchar_t* LevelEditorButtonW140 = 
		L"EngineContent/LevelEditor/ButtonW140.png";
	const wchar_t* LevelEditorButtonActivateDeactivateW140 = 
		L"EngineContent/LevelEditor/ActivateDeactivateButtonW140.png";
	const wchar_t* LevelEditorButtonW160 =
		L"EngineContent/LevelEditor/ButtonW160.png";
	const wchar_t* RenderLayerEyeIcon =
		L"EngineContent/LevelEditor/RenderLayerEyeIcon.png";
	// Gizmo
	const wchar_t* Gizmo = L"EngineContent/LevelEditor/Gizmo.png";
	//-----------------
};

// PLEASE NOTE! ALL THESE REMOVE COMPONENT FUNCTIONS ARE DEPRECIATED 
// AND REPLACED WITH "RemoveComponent" template function residing in engine class
// DELETE THESE FUNCTIONS LATER WHEN YOU HAVE CONFIRMED THAT THE TEMPLATE FUNCTION WORKS
//---------------------
// Remove bitmap component from "StoredBitmapComponents" 
extern "C" VOODOOENGINE_API void RemoveBitmapComponent(BitmapComponent* Component, VoodooEngine* Engine);
// Remove collision component from "StoredCollisionComponents"
extern "C" VOODOOENGINE_API void RemoveCollisionComponent(CollisionComponent* Component, VoodooEngine* Engine);
// Remove update component from "StoredUpdateComponents"
extern "C" VOODOOENGINE_API void RemoveUpdateComponent(UpdateComponent* Component, VoodooEngine* Engine);
// Remove input callback component from "InterfaceObjects_InputCallback"
extern "C" VOODOOENGINE_API void RemoveInputCallback(IInput* Component, VoodooEngine* Engine);
//----------------------------------------------

// Create the text format to be used by all direct write IU text for the remainder of the program
extern "C" VOODOOENGINE_API void CreateUITextFormat(VoodooEngine* Engine);
// Render UI text using direct write
extern "C" VOODOOENGINE_API void RenderUITextsRenderLayer(VoodooEngine* Engine);
// Print debug text to screen
extern "C" VOODOOENGINE_API void ScreenPrint(std::string DebugText, VoodooEngine* Engine);
//---------------------

// Gizmo
//---------------------
class Gizmo : public Object, public UpdateComponent, public IInput
{
public:
	VoodooEngine* Engine = nullptr;
	BitmapComponent GizmoBitmap;
	CollisionComponent GizmoCollision;

	// Will send event whenever a game object is moved by the gizmo
	std::vector<IEvent*> MoveGameObjectEventListeners;

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

	void InitGizmo(VoodooEngine* EngineReference)
	{
		Engine = EngineReference;
		SEditorAssetList Asset;
		GizmoBitmap.Bitmap = SetupBitmap(GizmoBitmap.Bitmap, Asset.Gizmo, Engine->Renderer);
		SetupBitmapComponent(&GizmoBitmap, GizmoBitmap.Bitmap);
		RenderGizmoCollisionRect = Engine->DebugMode;
		SetupGizmoCollisionTag();
		SetupGizmoCollisionRect();
		Engine->StoredEditorUpdateComponents.push_back(this);
		Engine->StoredEditorBitmapComponents.push_back(&GizmoBitmap);
		Engine->StoredEditorCollisionComponents.push_back(&GizmoCollision);
		Engine->InterfaceObjects_InputCallback.push_back(this);
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
				MouseClickLocationOffset.X) / Engine->LevelEditorGizmoSnapSize) * Engine->LevelEditorGizmoSnapSize;
			Location.Y =
				(int)((Engine->Mouse.Location.Y +
				MouseClickLocationOffset.Y) / Engine->LevelEditorGizmoSnapSize) * Engine->LevelEditorGizmoSnapSize;
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

			for (int i = 0; i < MoveGameObjectEventListeners.size(); i++)
			{
				MoveGameObjectEventListeners[i]->OnEventBroadcast();
			}
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
		for (int GameObjectIndex = 0; GameObjectIndex < Engine->StoredGameObjects.size(); GameObjectIndex++)
		{
			if (IsCollisionDetected(&Engine->Mouse.MouseCollider,
				&Engine->StoredGameObjects[GameObjectIndex]->AssetCollision))
			{
				CollisionDetected = true;
			}
		}

		return CollisionDetected;
	};

	int SetCurrentRenderLayerPrioritization(std::vector<GameObject*> CurrentGameObjectsFound)
	{
		std::vector<int> StoredRenderLayerIDs;
		for (int i = 0; i < CurrentGameObjectsFound.size(); i++)
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
		for (int IDIndex = 0; IDIndex < StoredRenderLayerIDs.size(); IDIndex++)
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
		for (int GameObjectIndex = 0; GameObjectIndex < GameObjectsFound.size(); GameObjectIndex++)
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
		Engine->Mouse.MouseHoveredObject = nullptr;
		std::vector<GameObject*> GameObjectsFound;
		for (int GameObjectIndex = 0; GameObjectIndex < Engine->StoredGameObjects.size(); GameObjectIndex++)
		{
			if (IsCollisionDetected(&Engine->Mouse.MouseCollider,
				&Engine->StoredGameObjects[GameObjectIndex]->AssetCollision))
			{
				GameObjectsFound.push_back(Engine->StoredGameObjects[GameObjectIndex]);
			}
		}

		if (GameObjectsFound.size() > 1)
		{
			//ScreenPrint("collided_with_more_than_one_object", Engine);

			Engine->Mouse.MouseHoveredObject = 
				AssignSelectedGameObjectByRenderLayer(
				GameObjectsFound, SetCurrentRenderLayerPrioritization(GameObjectsFound));
		}
		else if (!GameObjectsFound.empty())
		{
			//ScreenPrint("collided_with_one_object", Engine);

			Engine->Mouse.MouseHoveredObject = GameObjectsFound.at(0);
		}
	}

	void Update(float DeltaTime)
	{
		if (Engine->GameRunning)
		{
			return;
		}

		// Enables continous check for collision with game objects, 
		// instead of once per overlap/end overlap
		Engine->Mouse.MouseCollider.IsOverlapped = false;

		GizmoMouseHover = IsMouseHoveringGizmo();
		GameObjectMouseHover = IsMouseHoveringGameObject();
		UpdateMouseDragSnapLocationGizmo();
		UpdateGizmoLocation();
		UpdateSelectedGameObjectDragLocation();
	};

	SVector GetGizmoOffsetLocation()
	{
		SVector OffsetLocation = 
		{ (SelectedGameObject->AssetCollision.CollisionRect.X / 2) - 10,
			(SelectedGameObject->AssetCollision.CollisionRect.Y / 2) -
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

	void OnInputBroadcast(int Input, bool Pressed)
	{
		if (Engine->GameRunning)
		{
			FullGizmoReset();
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

			AssignMouseClickedGameObject();

			CurrentClickedGameObject = nullptr;
			if (GameObjectMouseHover &&
				Engine->Mouse.MouseHoveredObject != nullptr)
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
};
//---------------------

// File I/O
//-------------------------------------------
// Read only
extern "C" VOODOOENGINE_API bool SetDebugMode();
extern "C" VOODOOENGINE_API bool SetEditorMode();
// Saves the current open file as a "Lev" file
extern "C" VOODOOENGINE_API void SaveLevelFile(VoodooEngine* Engine);
// Opens windows file dialouge for which is used to select what level to open 
extern "C" VOODOOENGINE_API void OpenLevelFile(VoodooEngine* Engine);
// Read/Write (saving/loading)
extern "C" VOODOOENGINE_API void SaveGameObjectsToFile(char* FileName, VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void LoadGameObjectsFromFile(char* FileName, VoodooEngine* Engine);
//-------------------------------------------

// Level Editor
//-------------------------------------------
class LevelEditor : public Object, public UpdateComponent, public IInput, public IEvent
{

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
#define BUTTON_LOC_X_ASSETSELECTION 1740
#define BUTTON_LOC_Y_ASSETSELECTION 20
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

// This enum determine 
enum EMenuType
{
	None,
	AssetSelection,
	RenderLayerSelection,
	ViewModeSelection
};

public:
	LevelEditor(VoodooEngine* EngineReference)
	{
		Engine = EngineReference;

		// Add level editor update function to be called every frame
		Engine->StoredEditorUpdateComponents.push_back(this);

		// Add input callback for level editor
		Engine->InterfaceObjects_InputCallback.push_back(this);

		// Create level editor UI
		LevelEditorUITop.Bitmap = SetupBitmap(LevelEditorUITop.Bitmap, Asset.LevelEditorUITop, Engine->Renderer);
		SetupBitmapComponent(&LevelEditorUITop, LevelEditorUITop.Bitmap);
		LevelEditorUIOverlay.Bitmap = SetupBitmap(LevelEditorUIOverlay.Bitmap, Asset.LevelEditorUIOverlay, Engine->Renderer);
		SetupBitmapComponent(&LevelEditorUIOverlay, LevelEditorUIOverlay.Bitmap);
		Engine->StoredEditorBitmapComponents.push_back(&LevelEditorUIOverlay);
		Engine->StoredEditorBitmapComponents.push_back(&LevelEditorUITop);

		// Create all the clickable level editor buttons
		OpenLevelButton = CreateButton(
			Engine, OpenLevelButton, TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL,
			TwoSided, "open_level", { BUTTON_LOC_X_OPENLEVEL, BUTTON_LOC_Y_OPENLEVEL },
			Asset.LevelEditorButtonW140);
		SaveLevelButton = CreateButton(
			Engine, SaveLevelButton, TAG_LEVEL_EDITOR_BUTTON_SAVELEVEL,
			TwoSided, "save_level", { BUTTON_LOC_X_SAVELEVEL, BUTTON_LOC_Y_SAVELEVEL },
			Asset.LevelEditorButtonW140);
		CreatePlayLevelButton();
		PreviousButton = CreateButton(
			Engine, PreviousButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_PREVIOUS, 
			TwoSided, "previous", { BUTTON_LOC_X_PREVIOUS, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonW140);
		NextButton = CreateButton(
			Engine, NextButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_ASSET_LIST_NEXT, 
			TwoSided,  "next", { BUTTON_LOC_X_NEXT, BUTTON_LOC_Y_NEXT_PREVIOUS },
			Asset.LevelEditorButtonW140);
		AssetSelectionButton = CreateButton(
			Engine, AssetSelectionButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_ASSETS,
			TwoSided, "asset_select", { BUTTON_LOC_X_ASSETSELECTION, BUTTON_LOC_Y_ASSETSELECTION },
			Asset.LevelEditorButtonW160);
		RenderLayerSelectionButton = CreateButton(
			Engine, RenderLayerSelectionButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_RENDERLAYERS,
			TwoSided, "renderlayer", { BUTTON_LOC_X_RENDERLAYER, BUTTON_LOC_Y_RENDERLAYER },
			Asset.LevelEditorButtonW160);
		ViewModeSelectionButton = CreateButton(
			Engine, ViewModeSelectionButton, TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_VIEWMODE,
			TwoSided, "viewmode", { BUTTON_LOC_X_VIEWMODE, BUTTON_LOC_Y_VIEWMODE },
			Asset.LevelEditorButtonW140);

		// Create all render layer visibility eye icons 
		// (used to pick which render layer show/hide
		CreateRenderLayerVisiblityEyeIconButtons();
		// Create all clickable assets buttons
		CreateAssetButtons();
		// Display default asssets
		DisplayAssets(0);

		LevelEditorVisible = true;
		SetMenuVisible(EMenuType::ViewModeSelection);

		CreateGizmo();
	}

	Gizmo TransformGizmo;
	void(*ButtonPressedCallback)(int);
	bool LevelEditorVisible = false;
	bool AssetSelectionVisible = false;
	EMenuType CurrentMenuTypeActivated = EMenuType::None;

	// This is called whenever a game object is moved by the gizmo
	void OnEventBroadcast()
	{
		if (LevelEditorVisible)
		{
			UpdateSaveState(false);
		}
	};
	void CreateGizmo()
	{
		TransformGizmo.InitGizmo(Engine);
		TransformGizmo.InitGizmoLocation({ 1000, 500 });
		TransformGizmo.SetGizmoState(true);

		TransformGizmo.MoveGameObjectEventListeners.push_back(this);
	};
	void CreatePlayLevelButton()
	{
		PlayLevelButton = CreateButton(
			Engine, PlayLevelButton, TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL, 
			TwoSided, "play_level", { BUTTON_LOC_X_PLAYLEVEL, BUTTON_LOC_Y_PLAYLEVEL },
			Asset.LevelEditorButtonW140);
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
		// First get all stored game object and store them in "CurrentStoredButtonAssets"
		AddAssetContentForLevelEditor();

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
				CurrentStoredButtonAssets[i].AssetParams.EditorAssetButtonThumbnailFilePath);
			
			CurrentStoredButtonAssets[i].AssetButton = AssetButton;
			LocYOffset += OffsetYAmount;

			Index++;
		}
	};
	void CreateRenderLayerVisiblityEyeIconButtons()
	{
		SEditorAssetList AssetList;
		SVector OriginLocation = { 1620, 110 };
		float OffsetLocationY = 50;
		for (int i = 0; i < Engine->StoredRenderLayerUITexts.size(); i++)
		{
			auto Iterator = Engine->StoredRenderLayerUITexts.find(i);
			Button* EyeIcon = nullptr;
				EyeIcon = CreateButton(Engine, EyeIcon, Iterator->first,
				EButtonType::TwoSided, "", OriginLocation, AssetList.RenderLayerEyeIcon);
			RenderLayerVisibilityEyeIconButtons.push_back(EyeIcon);

			OriginLocation.Y += OffsetLocationY;
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

		std::vector<SAssetButton>().swap(CurrentStoredButtonAssets);

		if (CurrentStoredButtonAssets.empty())
		{
			//ScreenPrint("assets_empty", Engine);
		}
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
		for (int i = 0; i < CurrentStoredButtonAssets.size(); i++)
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
			if (!Engine->GameRunning)
			{
				MenuSelectedBeforeHidden = CurrentMenuTypeActivated;
			}

			HoveredButtonID = TAG_LEVEL_EDITOR_BUTTON_ID_NONE;
			LevelEditorVisible = false;
			LevelEditorUITop.BitmapParams.HiddenInGame = true;
			LevelEditorUIOverlay.BitmapParams.HiddenInGame = true;
			UpdateAllButtonsState(EButtonState::Hidden);
			SetMouseState(false, Engine);
		}
		else if (!Hide)
		{
			SetMouseState(true, Engine);
			LevelEditorVisible = true;
			LevelEditorUITop.BitmapParams.HiddenInGame = false;
			if (!Engine->GameRunning)
			{
				UpdateAllButtonsState(EButtonState::Default);
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

		for (int i = 0; i < Engine->StoredRenderLayerUITexts.size(); i++)
		{
			auto Iterator = Engine->StoredRenderLayerUITexts.find(i);
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
		// Never gets here
		case Disabled:
			break;
		case Hidden:
			SetToHide = true;
			DisableCollision = true;
			break;
		}

		for (int i = 0; i < RenderLayerVisibilityEyeIconButtons.size(); i++)
		{
			RenderLayerVisibilityEyeIconButtons[i]->ButtonBitmap.BitmapParams.HiddenInGame = SetToHide;
			RenderLayerVisibilityEyeIconButtons[i]->ButtonCollider.NoCollision = DisableCollision;
		}
	};
	void SetGameObjectsVisibilityBasedOnRenderLayer(bool EnableRenderLayer, int RenderLayer)
	{
		for (int i = 0; i < Engine->StoredGameObjects.size(); i++)
		{
			if (Engine->StoredGameObjects[i]->GameObjectBitmap.BitmapParams.RenderLayer == 
				RenderLayer)
			{
				if (EnableRenderLayer)
				{
					Engine->StoredGameObjects[i]->
						GameObjectBitmap.BitmapParams.HiddenInGame = false;
					Engine->StoredGameObjects[i]->
						AssetCollision.NoCollision = false;
				}
				else
				{
					Engine->StoredGameObjects[i]->
						GameObjectBitmap.BitmapParams.HiddenInGame = true;
					Engine->StoredGameObjects[i]->
						AssetCollision.NoCollision = true;
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
		auto Iterator = Engine->StoredRenderLayerUITexts.find(HoveredButtonID);
		if (Iterator->second.TextRenderType == ETextBrushRenderType::RenderBlackBrush)
		{
			// Show "on" eye icon bitmap
			SetBitmapSourceLocationX(
				&RenderLayerVisibilityEyeIconButtons.at(
				HoveredButtonID)->ButtonBitmap, BitmapWidth, 1);

			Iterator->second.TextRenderType = ETextBrushRenderType::RenderWhiteBrush;
			SetGameObjectsVisibilityBasedOnRenderLayer(true, HoveredButtonID);
		}
		else if (Iterator->second.TextRenderType == ETextBrushRenderType::RenderWhiteBrush)
		{
			// Show "off" eye icon bitmap
			SetBitmapSourceLocationX(
				&RenderLayerVisibilityEyeIconButtons.at(
				HoveredButtonID)->ButtonBitmap, BitmapWidth, 2);

			Iterator->second.TextRenderType = ETextBrushRenderType::RenderBlackBrush;
			SetGameObjectsVisibilityBasedOnRenderLayer(false, HoveredButtonID);
		}
	};
	void SetMenuVisible(EMenuType MenuToShow)
	{
		switch (MenuToShow)
		{
		case LevelEditor::None:
			AssetSelectionVisible = false;
			UpdateAssetThumbnailButtonsState(EButtonState::Hidden);
			SetButtonState(NextButton, EButtonState::Hidden);
			SetButtonState(PreviousButton, EButtonState::Hidden);
			SetButtonState(AssetSelectionButton, EButtonState::Hidden);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Hidden);
			SetButtonState(ViewModeSelectionButton, EButtonState::Hidden);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Hidden);
			SetAllRenderLayerUITextVisibility(false);
			LevelEditorUIOverlay.BitmapParams.HiddenInGame = true;
			break;
		case LevelEditor::AssetSelection:
			AssetSelectionVisible = true;
			UpdateAssetThumbnailButtonsState(EButtonState::Default);
			SetButtonState(NextButton, EButtonState::Default);
			SetButtonState(PreviousButton, EButtonState::Default);
			SetButtonState(AssetSelectionButton, EButtonState::Disabled);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Default);
			SetButtonState(ViewModeSelectionButton, EButtonState::Default);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Hidden);
			SetAllRenderLayerUITextVisibility(false);
			if (!Engine->GameRunning)
			{
				LevelEditorUIOverlay.BitmapParams.HiddenInGame = false;
			}
			break;
		case LevelEditor::RenderLayerSelection:
			AssetSelectionVisible = false;
			UpdateAssetThumbnailButtonsState(EButtonState::Hidden);
			SetButtonState(NextButton, EButtonState::Hidden);
			SetButtonState(PreviousButton, EButtonState::Hidden);
			SetButtonState(AssetSelectionButton, EButtonState::Default);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Disabled);
			SetButtonState(ViewModeSelectionButton, EButtonState::Default);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Default);
			SetAllRenderLayerUITextVisibility(true);
			if (!Engine->GameRunning)
			{
				LevelEditorUIOverlay.BitmapParams.HiddenInGame = false;
			}
			break;
		case LevelEditor::ViewModeSelection:
			AssetSelectionVisible = false;
			UpdateAssetThumbnailButtonsState(EButtonState::Hidden);
			SetButtonState(NextButton, EButtonState::Hidden);
			SetButtonState(PreviousButton, EButtonState::Hidden);
			SetButtonState(AssetSelectionButton, EButtonState::Default);
			SetButtonState(RenderLayerSelectionButton, EButtonState::Default);
			SetButtonState(ViewModeSelectionButton, EButtonState::Disabled);
			SetAllRenderLayerEyeIconButtonsState(EButtonState::Hidden);
			SetAllRenderLayerUITextVisibility(false);
			LevelEditorUIOverlay.BitmapParams.HiddenInGame = true;
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
			SaveLevelFile(Engine);
			SetButtonBitmapSourceClicked(SaveLevelButton);
			UpdateSaveState(true);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_OPENLEVEL:
			OpenLevelFile(Engine);
			UpdateSaveState(false);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_PLAYLEVEL:
			DeletePlayLevelButton();
			Engine->StartGame();
			TransformGizmo.SetGizmoState(true);
			MenuSelectedBeforeHidden = CurrentMenuTypeActivated;
			UpdateAllButtonsState(EButtonState::Hidden);
			CreateStopPlayButton();
			break;
		case TAG_LEVEL_EDITOR_BUTTON_STOPPLAY:
			DeleteStopPlayButton();
			CreatePlayLevelButton();
			Engine->EndGame();
			TransformGizmo.FullGizmoReset();
			UpdateAllButtonsState(EButtonState::Default);
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
			SetMenuVisible(EMenuType::AssetSelection);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_RENDERLAYERS:
			SetMenuVisible(EMenuType::RenderLayerSelection);
			break;
		case TAG_LEVEL_EDITOR_BUTTON_SELECT_MENU_VIEWMODE:
			SetMenuVisible(EMenuType::ViewModeSelection);
			break;
		default:
			// If no editor button is the button ID that was hovered, 
			// then this switch statement gets executed 
			switch (CurrentMenuTypeActivated)
			{
			// Do nothing
			case LevelEditor::None:
				break;
			// In asset menu mode, 
			// when asset button is clicked a game obejct based on ID is spawned
			case LevelEditor::AssetSelection: 
				if (Engine->AssetLoadFunctionPointer)
				{
					Engine->AssetLoadFunctionPointer(HoveredButtonID,
						{ ASSET_SELECTION_SPAWN_LOCATION_X, ASSET_SELECTION_SPAWN_LOCATION_Y });
					UpdateSaveState(false);
				}
				break;
			// In render layer selection mode,
			// when eye icon button is clicked, 
			// the game object assosiated with the eye icon's render layer,
			// will be toggled visible/hidden
			case LevelEditor::RenderLayerSelection:
				// We use toggle (on/off) for each eye icon button
				// the current state is determined internally for each eye icon button when clicked
				// so if button is clicked when set to "on" it will be set to "off" and vice versa
				SetRenderLayerEyeIconButtonState();
				break;
			}
			break;
		}
	};
	void OnInputBroadcast(int Input, bool Pressed)
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

		if (!LevelEditorVisible)
		{
			return;
		}

		if (Engine->Mouse.PrimaryMousePressed)
		{
			OnButtonPressed();
		}
		else if (!Engine->Mouse.PrimaryMousePressed)
		{
			if (AssetSelectionVisible)
			{
				ResetButtonsBitmapSource(PreviousButton);
				ResetButtonsBitmapSource(NextButton);
			}
		}

		if (Input == VK_DELETE &&
			TransformGizmo.SelectedGameObject != nullptr)
		{
			Engine->DeleteGameObject(TransformGizmo.SelectedGameObject);
			TransformGizmo.SelectedGameObject = nullptr;
			TransformGizmo.SetGizmoState(true);
			UpdateSaveState(false);
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
	void UpdateRenderLayerEyeIconButtonsCollisionCheck()
	{
		for (int i = 0; i < RenderLayerVisibilityEyeIconButtons.size(); i++)
		{
			UpdateButtonCollisionCheck(RenderLayerVisibilityEyeIconButtons[i]);
		}
	};
	void Update(float DeltaTime)
	{
		if (Engine == nullptr)
		{
			return;
		}

		UpdateButtonCollisionCheck(OpenLevelButton);
		UpdateButtonCollisionCheck(SaveLevelButton);
		UpdateButtonCollisionCheck(PlayLevelButton);
		UpdateButtonCollisionCheck(StopPlayButton);
		UpdateButtonCollisionCheck(PreviousButton);
		UpdateButtonCollisionCheck(NextButton);
		UpdateButtonCollisionCheck(AssetSelectionButton);
		UpdateButtonCollisionCheck(RenderLayerSelectionButton);
		UpdateButtonCollisionCheck(ViewModeSelectionButton);
		UpdateRenderLayerEyeIconButtonsCollisionCheck();

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
	bool ChangesMadeSinceLastSave = false;
	EMenuType MenuSelectedBeforeHidden = EMenuType::ViewModeSelection;
	BitmapComponent LevelEditorUITop;
	BitmapComponent LevelEditorUIOverlay;
	Button* OpenLevelButton = nullptr;
	Button* SaveLevelButton = nullptr;
	Button* PlayLevelButton = nullptr;
	Button* StopPlayButton = nullptr;
	Button* PreviousButton = nullptr;
	Button* NextButton = nullptr;
	Button* AssetSelectionButton = nullptr;
	Button* RenderLayerSelectionButton = nullptr;
	Button* ViewModeSelectionButton = nullptr;
	std::vector<Button*> RenderLayerVisibilityEyeIconButtons;

	void UpdateSaveState(bool Saved)
	{
		if (Saved)
		{
			ChangesMadeSinceLastSave = false;
			SetButtonState(SaveLevelButton, EButtonState::Disabled);
		}
		else
		{
			ChangesMadeSinceLastSave = true;
			SetButtonState(SaveLevelButton, EButtonState::Default);
		}
	};
	void AddLevelEditorAssetButton(int AssetID, const wchar_t* AssetPath, bool CreateAssetCollision)
	{
		SAssetButton AssetButton;
		AssetButton.AssetID = AssetID;
		AssetButton.AssetParams.EditorAssetButtonThumbnailFilePath = AssetPath;
		AssetButton.AssetParams.CreateDefaultAssetCollision = CreateAssetCollision;
		Engine->StoredButtonAssets.push_back(AssetButton);
	};
	void AddAssetContentForLevelEditor()
	{
		// Iterate over all stored game objects in the game and add them as asset buttons
		for (int i = 0; i < Engine->StoredGameObjectIDs.size(); i++)
		{
			auto Iterator = Engine->StoredGameObjectIDs.find(i);
			AddLevelEditorAssetButton(
				Iterator->first,
				Iterator->second.EditorAssetButtonThumbnailFilePath,
				Iterator->second.CreateDefaultAssetCollision);
		}

		// When above iteration is done, store all the buttons in the level editor,
		// which will then be used as clickable buttons
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

// Set the frame rate limit per second
extern "C" VOODOOENGINE_API void SetFPSLimit(VoodooEngine* Engine, float FPSLimit);

// Setup the application window and renderer
extern "C" VOODOOENGINE_API void InitWindow(
	VoodooEngine* Engine,
	LPCWSTR WindowTitle,
	WNDPROC WindowProcedure,
	int WindowResolutionWidth,
	int WindowResolutionHeight,
	bool WindowFullScreen = true);

// Setup the engine 
extern "C" VOODOOENGINE_API void InitEngine(VoodooEngine* Engine);
// Run the engine game loop
extern "C" VOODOOENGINE_API void RunEngine(VoodooEngine* Engine);

// Smooth interpolation from point A to B
extern "C" VOODOOENGINE_API float Interpolate(float Current, float Target, float DeltaTime, float Speed);

// Character stuff
//---------------------
class Character : public GameObject, public UpdateComponent
{
public:
	void OnGameObjectCreated(SVector SpawnLocation)
	{
		VoodooEngine::Engine->StoredUpdateComponents.push_back(this);
		OnGameObjectCreatedOverride(SpawnLocation);
	}
	void OnGameObjectDeleted()
	{
		VoodooEngine::Engine->RemoveComponent(
			(UpdateComponent*)this, &VoodooEngine::Engine->StoredUpdateComponents);
		OnGameObjectDeletedOverride();
	}
private:
	virtual void OnGameObjectCreatedOverride(SVector SpawnLocation){};
	virtual void OnGameObjectDeletedOverride(){};
};

// AI component that is used for directing AI characters 
class AIComponent
{
public:

};

// Add gravity to movement (only on Y axis)
extern "C" VOODOOENGINE_API float AddGravity(
	GameObject* ComponentOwner, MovementComponent& MoveComp, float GravityScale, float DeltaTime);

// Add movement input to game object. 
// Built in collision detection is provided,
// if you set up the "QuadCollisionParameters" struct within "MovementComponent".
// Returns new movement location
extern "C" VOODOOENGINE_API SVector AddMovementInput(
	GameObject* ComponentOwner, MovementComponent& MoveComp, VoodooEngine* Engine);

// Add AI movement to game object,
// If using AI then this will add movement to AI character using the assigned movement direction,
// built in collision where AI will stop and only resume movement until collision is not detected.
// Can assign what should be able to collide with AI and also connect an "OnCollided" event,
// returns new movement location
extern "C" VOODOOENGINE_API SVector AddMovementAI(AIComponent& AIComp);
//---------------------
