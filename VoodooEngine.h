#pragma once

// Win32 API
//---------------------
#include <Windows.h>

// Exclude rarely used stuff from Windows headers
//#define WIN32_LEAN_AND_MEAN
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
#include "SAsset.h"
#include "Text.h"
//---------------------

// includes indepentent from engine class
//---------------------
#include "DLevelEditorInfo.h"
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

	// Open file dialog box related, 
	// used for keeping track of file opened for saving
	std::wstring OpenedLevelFileString;

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
	// ID2D1Bitmap* TextureAtlasComponent
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
		if (VoodooEngine::Engine->PlayerStartObjectLeft)
		{
			if (VoodooEngine::Engine->PlayerStartObjectLeft->GameObjectBitmap.Bitmap)
			{
				VoodooEngine::Engine->PlayerStartObjectLeft->
					GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
			}
		}
		if (VoodooEngine::Engine->PlayerStartObjectRight)
		{
			if (VoodooEngine::Engine->PlayerStartObjectRight->GameObjectBitmap.Bitmap)
			{
				VoodooEngine::Engine->PlayerStartObjectRight->
					GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
			}
		}
		if (VoodooEngine::Engine->PlayerStartObjectUp)
		{
			if (VoodooEngine::Engine->PlayerStartObjectUp->GameObjectBitmap.Bitmap)
			{
				VoodooEngine::Engine->PlayerStartObjectUp->
					GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
			}
		}
		if (VoodooEngine::Engine->PlayerStartObjectDown)
		{
			if (VoodooEngine::Engine->PlayerStartObjectDown->GameObjectBitmap.Bitmap)
			{
				VoodooEngine::Engine->PlayerStartObjectDown->
					GameObjectBitmap.BitmapParams.BitmapSetToNotRender = !Show;
			}
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
			Iterator->second.TextureAtlasBitmap, 
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

	void SaveGameObjectsToFile(const wchar_t* FileName)
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

	void LoadGameObjectsFromFile(VoodooEngine* Engine,
		const wchar_t* FileName, std::vector<GameObject*>& LevelToAddGameObject, bool DeleteExistingObjectsOnLoad = true)
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

	void LoadLevelFromFile(VoodooEngine* Engine, 
		const wchar_t* FilePath, std::vector<GameObject*>& LevelToAddGameObjects)
	{
		LoadGameObjectsFromFile(Engine, FilePath, LevelToAddGameObjects, false);
	}

	void SaveLevelFile()
	{
		SaveGameObjectsToFile(VoodooEngine::Engine->OpenedLevelFileString.c_str());
	}
};

extern "C" VOODOOENGINE_API void OpenLevelFile(VoodooEngine* Engine);

#include "Gizmo.h"

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
