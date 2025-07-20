#pragma once

// Win32 API
//---------------------
#include <Windows.h>

// Exclude rarely used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
//---------------------

#include <commdlg.h>
#include <fstream>
#include <sstream>
#include <map>

// Disable warning of using "wcstombs"
#pragma warning(disable:4996)

// includes engine class is dependent of 
//---------------------
#include "CollisionComponent.h"
#include "UpdateComponent.h"
#include "BitmapComponent.h"
#include "Interface.h"
#include "Renderer.h"
#include "Button.h"
#include "Asset.h"
#include "Text.h"
//---------------------

// includes indepentent from engine class
//---------------------
#include "AIComponent.h"
#include "Interpolate.h"
#include "PlayerStart.h"
#include "Animation.h"
//---------------------

// VoodooEngine is a complete engine framework for making a 2D game
// --------------------
// Supported features:
// 
// GRAPHICS
// - Creation of an application window with custom title name and icon using "Win32 API"
// - Bitmap creation from png file using "Direct2D API"
// - Bitmap rendering using "Direct2D API"
// - Spritesheet animation
// - Text rendering using "DirectWrite API"
// 
// INPUT
// - User input for keyboard using "Win32 API" 
// - User input for gamepad using "XInput API" 
// 
// COLLISION
// - Collision detection using "AABB" algorithm
// 
// GRAVITY
// - Gravity with velocity
// 
// SOUND
// - Playing one shot sounds/looping sounds using "XAudio2 API"
//
// UPDATE 
// - Update function with deltatime
// 
// TIMER
// - Timer countdown with function pointer callback when finished
// 
// SPAWN/DELETE GAMEOBJECTS 
// - Creating gameobjects dynamically during gameplay using base gameobject class
// 
// INTERFACES
// - IRender
// - IInput
// - IGameState
// - ILevelActivated
// 
// LEVEL
// - Level editor 
// - Level loading/unloading
// 
// TRIGGER
// - Triggers with begin/end overlapping events
// 
// SAVE/LOAD
// - Saving/loading from files
// --------------------

// Naming conventions
// --------------------
// 
// For class names never make a name shortened, e.g. writing "Params" instead of "Parameters"
// 
// PREFIX INITIALS
// - "S" stands for "Struct" e.g. "SVector", "SColor"
// - "E" stands for "enum" e.g. "EButtonType"
// - "I" stands for interface e.g. IInteract, IDamage
// 
// PREFIX INTERFACE
// - Every interface vector containers needs to have prefix "InterfaceObjects_" e.g. "InterfaceObjects_Input"
// - Every interfaces inherited virtual functions, 
// needs to have have prefix of "InterfaceEvent_" e.g. "InterfaceEvent_Input"
// - Every function that sends input internally in the engine, 
// needs to have prefix "SendInterface_" e.g. "SendInterface_Input"
// 
// PREFIX FUNCTION POINTERS
// Function pointers needs to have prefix "FunctionPointer_" e.g. "FunctionPointer_LoadGameObjects"
// 
// PREFIX "UPDATE"
// Functions names with prefix "update" naming rule
// - Every function name with prefix "Update" e.g. "UpdateAnimation", must be called every frame, 
// NOTE never name anything with "Update" as prefix if you are not going to call it every frame 
// --------------------

// All keyboard keybind ID's
#define INPUT_KEY_A 0x41
#define INPUT_KEY_B 0x42
#define INPUT_KEY_C 0x43
#define INPUT_KEY_D 0x44
#define INPUT_KEY_E 0x45
#define INPUT_KEY_F 0x46
#define INPUT_KEY_G 0x47
#define INPUT_KEY_H 0x48
#define INPUT_KEY_I 0x49
#define INPUT_KEY_J 0x4A
#define INPUT_KEY_K 0x4B
#define INPUT_KEY_L 0x4C
#define INPUT_KEY_M 0x4D
#define INPUT_KEY_N 0x4E
#define INPUT_KEY_O 0x4F
#define INPUT_KEY_P 0x50
#define INPUT_KEY_Q 0x51
#define INPUT_KEY_R 0x52
#define INPUT_KEY_S 0x53 
#define INPUT_KEY_T 0x54
#define INPUT_KEY_U 0x55
#define INPUT_KEY_V 0x56
#define INPUT_KEY_W 0x57
#define INPUT_KEY_X 0x58
#define INPUT_KEY_Y 0x59
#define INPUT_KEY_Z 0x5A
#define INPUT_KEY_ARROW_UP 0x26
#define INPUT_KEY_ARROW_DOWN 0x28
#define INPUT_KEY_ARROW_LEFT 0x25
#define INPUT_KEY_ARROW_RIGHT 0x27
#define INPUT_KEY_SPACE 0x20
#define INPUT_KEY_ENTER 0x0D
#define INPUT_KEY_TAB 0x09
#define INPUT_KEY_SHIFT_LEFT 0xA0 
#define INPUT_KEY_SHIFT_RIGHT 0xA1
#define INPUT_KEY_CTRL_LEFT 0xA2
#define INPUT_KEY_CTRL_RIGHT 0xA3

// Window parameters information i.e. title name of application, screen size, fullscreen/border windowed etc.  
struct SWindowParameters
{
	HWND HWind = nullptr;
	WNDCLASSEX WindowClass;
	LPCWSTR WindowTitle;
	int ScreenResolutionWidth;
	int ScreenResolutionHeight;
	bool Fullscreen = true;
};

// Windows procedure parameters information used for input check from windows operating system
struct SWindowsProcedureParameters
{
	HWND HWind;
	UINT Message;
	WPARAM WParam;
	LPARAM LParam;
};

// Mouse class contains all that is needed for custom mouse cursor support in the engine
class VoodooMouse : public Object
{
public:
	BitmapComponent MouseBitmap;
	CollisionComponent MouseCollider;
	Object* MouseHoveredObject = nullptr;
	bool PrimaryMousePressed = false;
	bool SecondaryMousePressed = false;
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
	SWindowParameters Window;
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
	void(*FunctionPointer_LoadGameObjects)(int, SVector, std::vector<GameObject*>&) = nullptr;

	std::vector<IRender*> InterfaceObjects_Render;
	std::vector<IInput*> InterfaceObjects_Input;
	std::vector<IGameState*> InterfaceObjects_GameState;
	std::vector<ILevelActivated*> InterfaceObjects_LevelActivated;

	// Optional level background that can be used in game levels
	// (Note this bitmap will always be set to render first in the painter's algorithm, 
	// which means it will always be in the background and everything else will will be rendered on top of it)
	BitmapComponent* CurrentLevelBackground = nullptr;

	// Player start game objects 
	// (used to teleport player to the assigned player start object location during gameplay)
	GameObject* PlayerStartObjectLeft = nullptr;
	GameObject* PlayerStartObjectRight = nullptr;
	GameObject* PlayerStartObjectUp = nullptr;
	GameObject* PlayerStartObjectDown = nullptr;

	// This asset texture atlas map is used to store all the asset texture atlases used in the game,
	// The map value is used to assing an asset texture atlas to a game object ID
	std::map<int, SAssetTextureAtlas> StoredAssetTextureAtlases;

	// The asset parameter struct contains variables in this order:
	// ID2D1Bitmap* TextureAtlas
	// SVector TextureAtlasWidthHeight
	// int TextureAtlasOffsetMultiplierHeight
	// int RenderLayer
	// bool CreateDefaultAssetCollision
	// const wchar_t* AssetFilePath
	// float EditorAssetButtonThumbnailTextureAtlasHeight
	// float EditorAssetButtonThumbnailTextureAtlasOffsetMultiplierY
	std::map<int, SAssetParameters> StoredGameObjectIDs;
	
	// Stored game object related vectors
	std::vector<BitmapComponent*> StoredBitmapComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<GameObject*> StoredGameObjects;
	std::vector<UpdateComponent*> StoredUpdateComponents;

	// Stored timer update components (exlusive for timers)
	std::vector<UpdateComponent*> StoredTimerUpdateComponents;

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
	// the brushes will be created on init and be available for the remainder of the program 
	// to any text instances
	IDWriteTextFormat* TextFormat = nullptr;
	ID2D1SolidColorBrush* BlackBrush = nullptr;
	ID2D1SolidColorBrush* WhiteBrush = nullptr;
	// Used to store all UI text for render layers using directwrite
	std::map<int, STextParameters> StoredLevelEditorRenderLayers;

	// Clear all debug text from screen
	static void ClearScreenPrint(VoodooEngine* Engine)
	{
		if (Engine->StoredScreenPrintTexts.empty())
		{
			return;
		}

		while (!Engine->StoredScreenPrintTexts.empty())
		{
			for (int i = 0; i < Engine->StoredScreenPrintTexts.size(); ++i)
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

	// Sends interface input event whenever an input is updated
	static void SendInterface_Input(VoodooEngine* Engine, int Input, bool Pressed)
	{
		for (int i = 0; i < Engine->InterfaceObjects_Input.size(); ++i)
		{
			Engine->InterfaceObjects_Input[i]->InterfaceEvent_Input(Input, Pressed);
		}
	}

	static void SetMouseState(VoodooEngine* Engine, bool Show)
	{
		if (Show)
		{
			Engine->Mouse.MouseCollider.NoCollision = false;
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapSetToNotRender = false;

			if (Engine->DebugMode ||
				!Engine->Mouse.MouseBitmap.Bitmap)
			{
				Engine->Mouse.MouseCollider.RenderCollisionRect = true;
			}
		}
		else
		{
			// Only allow mouse to be hidden if game is running
			if (!Engine->GameRunning)
			{
				return;
			}

			Engine->Mouse.MouseCollider.NoCollision = true;
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapSetToNotRender = true;
			Engine->Mouse.MouseCollider.RenderCollisionRect = false;

			Engine->Mouse.MouseCollider.CollisionRectColor = { 0.2, 0.5, 0 };
		}
	}

	static void UpdateMouseInput(VoodooEngine* Engine, UINT Message)
	{
		switch (Message)
		{
		// Primary mouse button
		case WM_LBUTTONDOWN:
			Engine->Mouse.PrimaryMousePressed = true;
			SendInterface_Input(Engine, WM_LBUTTONDOWN, true);
			break;
		case WM_LBUTTONUP:
			Engine->Mouse.PrimaryMousePressed = false;
			SendInterface_Input(Engine, WM_LBUTTONUP, false);
			break;
		// Secondary mouse button
		case WM_RBUTTONDOWN:
			Engine->Mouse.SecondaryMousePressed = true;
			SendInterface_Input(Engine, WM_RBUTTONDOWN, true);
			break;
		case WM_RBUTTONUP:
			Engine->Mouse.SecondaryMousePressed = false;
			SendInterface_Input(Engine, WM_RBUTTONUP, false);
			break;
		}
	};

	static void UpdateKeyboardInput(VoodooEngine* Engine, UINT Message, WPARAM WParam)
	{
		switch (Message)
		{
		case WM_KEYDOWN:
			SendInterface_Input(Engine, WParam, true);
			break;
		case WM_KEYUP:
			SendInterface_Input(Engine, WParam, false);
			break;
		default:
			break;
		}
	}

	inline static LRESULT CALLBACK WindowsProcedure(
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

	void SetPlayerStartObjectsVisibility(bool Show)
	{
		if (PlayerStartObjectLeft)
		{
			PlayerStartObjectLeft->GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
		}
		if (PlayerStartObjectRight)
		{
			PlayerStartObjectRight->GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
		}
		if (PlayerStartObjectUp)
		{
			PlayerStartObjectUp->GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
		}
		if (PlayerStartObjectDown)
		{
			PlayerStartObjectDown->GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
		}
	}

	void StartGame()
	{
		GameRunning = true;
		for (int i = 0; i < StoredGameObjects.size(); ++i)
		{
			// If no default asset collision is assigned, 
			// then disable collision when game starts
			if (!StoredGameObjects[i]->CreateDefaultGameObjectCollisionInGame)
			{
				StoredGameObjects[i]->DefaultGameObjectCollision.NoCollision = true;
			}
		}
		for (int i = 0; i < InterfaceObjects_GameState.size(); ++i)
		{
			InterfaceObjects_GameState[i]->InterfaceEvent_GameStart();
		}

		SetPlayerStartObjectsVisibility(false);
	}
	void EndGame()
	{
		GameRunning = false;
		for (int i = 0; i < StoredGameObjects.size(); ++i)
		{
			// If no default asset collision is assigned, 
			// then enable collision when game ends 
			// (so the asset is clickable in the editor during level edit)
			if (!StoredGameObjects[i]->CreateDefaultGameObjectCollisionInGame)
			{
				StoredGameObjects[i]->DefaultGameObjectCollision.NoCollision = false;
			}
		}
		for (int i = 0; i < InterfaceObjects_GameState.size(); ++i)
		{
			InterfaceObjects_GameState[i]->InterfaceEvent_GameEnd();
		}

		SetPlayerStartObjectsVisibility(true);
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
		StoredGameObjects.back()->CreateDefaultGameObjectCollisionInGame = 
			Iterator->second.CreateDefaultAssetCollision;
		SetupBitmapComponent(&StoredGameObjects.back()->GameObjectBitmap, 
			Iterator->second.TextureAtlas, 
			Iterator->second.TextureAtlasWidthHeight, 
			Iterator->second.TextureAtlasOffsetMultiplierHeight, false);
		StoredGameObjects.back()->GameObjectDimensions.X = Iterator->second.TextureAtlasWidthHeight.X;
		StoredGameObjects.back()->GameObjectDimensions.Y = Iterator->second.TextureAtlasWidthHeight.Y;
		StoredGameObjects.back()->GameObjectBitmap.BitmapParams.RenderLayer = Iterator->second.RenderLayer;
		StoredGameObjects.back()->GameObjectBitmap.ComponentLocation = SpawnLocation;
		StoredBitmapComponents.push_back(&StoredGameObjects.back()->GameObjectBitmap);

		// If in editor mode, create a clickable collision rect for the spawned game object 
		// in order for it to be selectable in the level editor
		if (EditorMode || 
			Iterator->second.CreateDefaultAssetCollision)
		{
			StoredGameObjects.back()->DefaultGameObjectCollision.CollisionRect =
				{ Iterator->second.TextureAtlasWidthHeight.X, Iterator->second.TextureAtlasWidthHeight.Y };
			StoredGameObjects.back()->DefaultGameObjectCollision.ComponentLocation = SpawnLocation;
			StoredGameObjects.back()->DefaultGameObjectCollision.CollisionTag = GameObjectID;
			StoredGameObjects.back()->DefaultGameObjectCollision.Owner = StoredGameObjects.back();
			// Only set to render collision rect if in debug mode
			if (DebugMode)
			{
				StoredGameObjects.back()->DefaultGameObjectCollision.RenderCollisionRect = true;
				StoredGameObjects.back()->DefaultGameObjectCollision.CollisionRectColor = EditorCollisionRectColor;
			}
			StoredCollisionComponents.push_back(&StoredGameObjects.back()->DefaultGameObjectCollision);
		}
		StoredGameObjects.back()->OnGameObjectCreated(SpawnLocation);
		return (T*)StoredGameObjects.back();
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
			ClassToDelete->CreateDefaultGameObjectCollisionInGame)
		{
			RemoveComponent(&ClassToDelete->DefaultGameObjectCollision, &this->StoredCollisionComponents);
		}

		// Custom optional deconstructor called before delete
		// in case for example you want to delete anything custom made
		// created outside of the default game object base class stuff 
		// such as "GameObjectBitmap", "DefaultGameObjectCollision" etc.
		ClassToDelete->OnGameObjectDeleted();
		delete ClassToDelete;
		ClassToDelete = nullptr;
		return nullptr;
	};

	void DeleteAllGameObjects()
	{
		while (!StoredGameObjects.empty())
		{
			for (int i = 0; i < StoredGameObjects.size(); ++i)
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

	void SaveGameObjectsToFile(char* FileName)
	{
		std::ofstream File(FileName);
		if (File.is_open())
		{
			for (int i = 0; i < Engine->StoredGameObjects.size(); ++i)
			{
				File << StoredGameObjects[i]->GameObjectID
					<< " " << Engine->StoredGameObjects[i]->Location.X
					<< " " << Engine->StoredGameObjects[i]->Location.Y << '\n';
			}
		}

		File.close();
	}

	void LoadGameObjectsFromFile(char* FileName,
		std::vector<GameObject*>& LevelToAddGameObject, VoodooEngine* Engine, bool DeleteExistingObjectsOnLoad = true)
	{
		if (DeleteExistingObjectsOnLoad)
		{
			// Delete all current game objects
			Engine->DeleteAllGameObjects();
		}

		std::fstream File(FileName);
		if (File.is_open())
		{
			int GameObjectID = 0;
			SVector SpawnLocation = {};

			std::string VerticalLine;
			while (getline(File, VerticalLine))
			{
				std::stringstream Stream(VerticalLine);
				std::string HorizontalLine;
				std::vector<std::string> HorizontalLineNum;
				while (Stream >> HorizontalLine)
				{
					HorizontalLineNum.push_back(HorizontalLine);
				}
				if (HorizontalLineNum.empty())
				{
					File.close();
					return;
				}

				GameObjectID = (std::stoi(HorizontalLineNum[0]));
				SpawnLocation.X = (std::stof(HorizontalLineNum[1]));
				SpawnLocation.Y = (std::stof(HorizontalLineNum[2]));

				FunctionPointer_LoadGameObjects(GameObjectID, SpawnLocation, LevelToAddGameObject);
			}
		}

		File.close();
	}

	void LoadLevelFromFile(const wchar_t* FilePath, std::vector<GameObject*>& LevelToAddGameObjects, VoodooEngine* Engine)
	{
		char* LevelFileName = new char[100];
		wcstombs(LevelFileName, FilePath, 100);
		LoadGameObjectsFromFile(LevelFileName, LevelToAddGameObjects, Engine, false);
		delete[]LevelFileName;
	}
};

// Print debug text to screen
extern "C" VOODOOENGINE_API void ScreenPrint(VoodooEngine* Engine, std::string DebugText);

// Set the frame rate limit per second
extern "C" VOODOOENGINE_API void SetFPSLimit(VoodooEngine* Engine, float FPSLimit);

// Pause/unpause game
extern "C" VOODOOENGINE_API void PauseGame(VoodooEngine* Engine, bool SetGamePaused);

// includes that are dependent of engine class 
// (the engine class needs to be created above first before includes)
//---------------------
#include "MovementComponent.h"
#include "VoodooLevelEditor.h"
#include "TimerHandle.h"
#include "Character.h"
#include "Trigger.h"
#include "LoadLevelTrigger.h"
//---------------------

// Setup the application window and renderer
extern "C" VOODOOENGINE_API void InitWindowAndRenderer(
	VoodooEngine* Engine,
	LPCWSTR WindowTitle,
	WNDPROC WindowsProcedure,
	int WindowResolutionWidth,
	int WindowResolutionHeight,
	bool WindowFullScreen = true);

// Setup the engine 
extern "C" VOODOOENGINE_API void InitEngine(VoodooEngine* Engine, SRenderLayerNames RenderLayerNames);
// Run the engine game loop
extern "C" VOODOOENGINE_API void RunEngine(VoodooEngine* Engine);

// Activate a desired level
extern "C" VOODOOENGINE_API	void ActivateLevel(
	VoodooEngine* Engine,
	std::vector<GameObject*>& Level,
	int PlayerID = -1,
	int PlayerStartLeftID = -1,
	int PlayerStartRightID = -1,
	int PlayerStartUpID = -1,
	int PlayerStartDownID = -1,
	BitmapComponent* LevelBackground = nullptr);

// Set the location of gameobjects
extern "C" VOODOOENGINE_API void SetGameObjectLocation(GameObject* GameObjectToSet, SVector NewLocation);

// Set the location of gameobjects that inherit from character class
extern "C" VOODOOENGINE_API void SetCharacterLocation(Character* CharacterToSet, SVector NewLocation);

// Add movement input to game object. 
// Built in collision detection is provided,
// if you set up the "QuadCollisionParameters" struct within "MovementComponent".
// Returns new movement location
extern "C" VOODOOENGINE_API SVector AddMovementInput(VoodooEngine* Engine, Character* CharacterToAddMovement);

// Add AI movement to game object 
// (will inherit from "MovementComponent" and make use of "AddMovementInput" function)
// If using AI then this will add movement to AI character using the assigned movement direction,
// built in collision where AI will stop and only resume movement until collision is not detected.
// Can assign what should be able to collide with AI and also connect an "OnCollided" event,
// returns new movement location
extern "C" VOODOOENGINE_API SVector AddMovementAI(AIComponent& AIComp);
