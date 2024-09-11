#pragma once

// STL
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
// Setup window class for preparing for window creation
extern "C" VOODOOENGINE_API void SetupWindowParams(WNDCLASSEX &WindowClass, 
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
class InputCallback
{
public:
	virtual void BroadcastInput(int Input, bool Pressed){};
};
// Check if input is being pressed/released, returns a bool and is false if no input is found
extern "C" VOODOOENGINE_API bool InputPressed(std::map<int, bool> StoredInputs, int InputToCheck);
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
	int R = 0;
	int G = 0;
	int B = 0;
};
//-------------------------------------------

// Base component that all movable components inherit
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
// Bitmap component class
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
//-------------------------------------------

// Collision detection
//-------------------------------------------
// Collision component class
class CollisionComponent : public TransformComponent
{
public:
	bool NoCollision = false;
	int CollisionTag = 0;
	std::vector<int> CollisionTagsToIgnore;
	SColor CollisionRectColor = {0,0,0};
	SVector CollisionRect = {0,0};
	SVector CollisionRectOffset = {0,0};

	virtual void OnBeginOverlap(){};
	virtual void OnEndOverlap(){};
};
//-------------------------------------------

// Renderer
//-------------------------------------------
extern "C" VOODOOENGINE_API ID2D1HwndRenderTarget* SetupRenderer(
	ID2D1HwndRenderTarget* RenderTarget, HWND WindowHandle);
extern "C" VOODOOENGINE_API void RenderByLayer(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> StoredBitmaps, int RenderLayer);
extern "C" VOODOOENGINE_API void Render(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> BitmapsToRender, 
	int MaxNumRenderLayers);
extern "C" VOODOOENGINE_API void RenderCollisionRectangles(
	ID2D1HwndRenderTarget* Renderer, std::vector<CollisionComponent*> CollisionRectsToRender);
//-------------------------------------------

// Update component
//-------------------------------------------
class UpdateComponent
{
public:
	virtual void Update(float DeltaTime) = 0;
};
//-------------------------------------------

// Object
//-------------------------------------------
// Base object class inherited by all objects in the game
//-------------------------------------------
class Object
{
public:
	SVector Location = {0,0};
};
extern "C" VOODOOENGINE_API SVector GetObjectLocation(Object* ObjectToSearch);
extern "C" VOODOOENGINE_API void SetObjectLocation(Object* ObjectToUpdate, SVector NewLocation);
extern "C" VOODOOENGINE_API void SetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* ComponentToUpdate, SVector NewLocation);
extern "C" VOODOOENGINE_API SVector GetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* ComponentToSearch);
//-------------------------------------------

// Voodoo engine class
//-------------------------------------------
class VoodooEngine : public InputCallback
{
public:
	bool EngineRunning = false;
	SWindowParams Window;
	ID2D1HwndRenderTarget* Renderer = nullptr;
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER EndCounter;
	LARGE_INTEGER Counts;
	LARGE_INTEGER Frequency;
	LARGE_INTEGER FPS;
	float TargetSecondsPerFrame = 1.0f / 80.0f;
	float DeltaTime = 0;
	std::vector<Object*> StoredObjects;
	std::vector<UpdateComponent*> StoredUpdateComponents;
	std::vector<CollisionComponent*> StoredCollisionComponents;
	std::vector<BitmapComponent*> StoredBitmaps;
	std::map<int, bool> StoredInputs;
	std::vector<InputCallback*> StoredInputCallbacks;
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
extern "C" VOODOOENGINE_API float SetNewFPSLimit(float NewFPSLimit);
extern "C" VOODOOENGINE_API float UpdateFrameRate(VoodooEngine* Engine);
//-------------------------------------------
