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
// Setup app window parameters preparing for window creation
extern "C" VOODOOENGINE_API void SetupAppWindowParams(WNDCLASSEX &WindowClass, 
	WNDPROC InputCallbackFunction);
// Create window and return window handle
extern "C" VOODOOENGINE_API void CreateAppWindow(SWindowParams &WindowParams);
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
	SVector ComponentLocation = {0,0};
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
	SVector BitmapOffsetLeft = {0,0};
	SVector BitmapOffsetRight = {0,0};
	SVector BitmapSource = {0,0};
};
// Bitmap component
class BitmapComponent : public TransformComponent
{
public:
	~BitmapComponent()
	{
		Bitmap = nullptr;
	}

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
	SVector Location = {0,0};

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
	SColor CollisionRectColor = {255, 255, 255};
	SVector CollisionRect = {0,0};
	SVector CollisionRectOffset = {0,0};
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
	std::vector<Object*> StoredObjects;
	std::vector<UpdateComponent*> StoredUpdateComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<BitmapComponent*> StoredBitmapComponents;
	std::map<int, bool> StoredInputs;
	std::vector<InputCallback*> StoredInputCallbacks;

	// Global font asset paths
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
//-------------------------------------------

// Button
//-------------------------------------------
struct ButtonParameters
{
	const wchar_t* AssetPathButtonBitmap = L"";
	int ButtonCollisionTag = 0;
	int ButtonWidth = 140;
	std::string ButtonTextString = "";
	SVector ButtonTextOffset = {-2, 10};
	SVector ButtonLocation = {0,0};
};
class Button
{
public:
	CollisionComponent* ButtonCollider = nullptr;
	BitmapComponent* ButtonBitmap = nullptr;
	ButtonParameters ButtonParams = {};
};
extern "C" VOODOOENGINE_API Button* CreateButton(
	VoodooEngine* Engine, Button* ButtonToCreate, ButtonParameters ButtonParams);
extern "C" VOODOOENGINE_API void DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete);
//-------------------------------------------

// Create/Delete
//-------------------------------------------
extern "C" VOODOOENGINE_API void DeleteBitmapComponent(VoodooEngine* Engine, BitmapComponent* Component);
extern "C" VOODOOENGINE_API void DeleteCollisionComponent(VoodooEngine* Engine, CollisionComponent* Component);
extern "C" VOODOOENGINE_API void ScreenPrint(std::string DebugText, VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void ClearScreenPrint(VoodooEngine* Engine);
extern "C" VOODOOENGINE_API void CloseApp(VoodooEngine* Engine);
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
