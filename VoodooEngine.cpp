#include "pch.h"
#include "VoodooEngine.h"
#include <fstream>
#include <sstream>
#include <string>

void SetupAppWindowParams(WNDCLASSEX &WindowClass, WNDPROC InputCallbackFunction)
{
	LPCWSTR ClassName = L"Class";
	HINSTANCE HInstance = nullptr;
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.lpfnWndProc = InputCallbackFunction;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = HInstance;
	WindowClass.lpszClassName = ClassName;
	WindowClass.lpszMenuName = nullptr;
	WindowClass.hbrBackground = nullptr;
	WindowClass.hIcon = nullptr;
	WindowClass.hIconSm = nullptr;
	WindowClass.hCursor = nullptr;

	RegisterClassEx(&WindowClass);
}

void CreateAppWindow(SWindowParams &WindowParams)
{
	WindowParams.HWind = CreateWindow(
		WindowParams.WindowClass.lpszClassName,
		WindowParams.WindowTitle,
		WindowParams.Fullscreen,
		0, 0,
		WindowParams.ScreenResolutionWidth,
		WindowParams.ScreenResolutionHeight,
		nullptr,
		nullptr,
		WindowParams.WindowClass.hInstance,
		nullptr);
}

void SetCustomAppIcon(LPCWSTR IconFileName, HWND &HWind)
{
	// Load custom app icon from file (ico file format)
	HANDLE CustomAppIcon = LoadImage(
		0, IconFileName,
		IMAGE_ICON, 0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE);

	// Assign the custom icon as the official icon of the app 
	// (if no icon is found, then the engine will use default desktop icon)
	if (CustomAppIcon)
	{
		SendMessage(HWind, WM_SETICON, ICON_SMALL, (LPARAM)CustomAppIcon);
		SendMessage(HWind, WM_SETICON, ICON_BIG, (LPARAM)CustomAppIcon);
		
		SendMessage(GetWindow(HWind, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)CustomAppIcon);
		SendMessage(GetWindow(HWind, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)CustomAppIcon);
	}
}

void UpdateAppWindow()
{
	MSG MSGMessage;
	MSGMessage.message = WM_NULL;
	while (PeekMessage(&MSGMessage, nullptr, 0, 0, PM_REMOVE))
		DispatchMessage(&MSGMessage);
}

bool InputPressed(std::map<int, bool> StoredInputs, int InputToCheck)
{
	int KeyToCheck = InputToCheck;
	auto Iterator = StoredInputs.find(KeyToCheck);
	if (Iterator != StoredInputs.end())
		return Iterator->second;
 
	return false;
}

void BroadcastInput(std::vector<InputCallback*> StoredCallbacks, int Input, bool Pressed)
{
	for (int i = 0; i < StoredCallbacks.size(); i++)
	{
		StoredCallbacks[i]->BroadcastInput(Input, Pressed);
	}
}

float GetSecondsPerFrame(
	LARGE_INTEGER* StartCounter, LARGE_INTEGER* EndCounter, LARGE_INTEGER* Frequency)
{
	return ((float)(EndCounter->QuadPart - StartCounter->QuadPart) / (float)Frequency->QuadPart);
}

void SetFPSLimit(VoodooEngine* Engine, float FPSLimit)
{
	Engine->TargetSecondsPerFrame = 1 / FPSLimit;
}

float UpdateFrameRate(VoodooEngine* Engine)
{
	QueryPerformanceCounter(&Engine->EndCounter);
	float SecondsPerFrame = GetSecondsPerFrame(
		&Engine->StartCounter, &Engine->EndCounter, &Engine->Frequency);

	if (SecondsPerFrame < Engine->TargetSecondsPerFrame)
	{
		DWORD sleep_ms;
		sleep_ms = (DWORD)(1000 * (Engine->TargetSecondsPerFrame - SecondsPerFrame));
		Sleep(sleep_ms);
		while (SecondsPerFrame < Engine->TargetSecondsPerFrame)
		{
			QueryPerformanceCounter(&Engine->EndCounter);
			SecondsPerFrame = GetSecondsPerFrame(
				&Engine->StartCounter, &Engine->EndCounter, &Engine->Frequency);
		}
	}

	QueryPerformanceCounter(&Engine->EndCounter);
	SecondsPerFrame = GetSecondsPerFrame(
		&Engine->StartCounter, &Engine->EndCounter, &Engine->Frequency);

	Engine->Counts.QuadPart = Engine->EndCounter.QuadPart - Engine->StartCounter.QuadPart;
	Engine->FPS.QuadPart = Engine->Frequency.QuadPart / Engine->Counts.QuadPart;
	Engine->StartCounter = Engine->EndCounter;

	return SecondsPerFrame;
}

void DeleteBitmapComponent(VoodooEngine* Engine, BitmapComponent* Component)
{
	Engine->StoredBitmapComponents.erase(std::remove(
		Engine->StoredBitmapComponents.begin(),
		Engine->StoredBitmapComponents.end(), Component));
}

void DeleteCollisionComponent(VoodooEngine* Engine, CollisionComponent* Component)
{
	Engine->StoredCollisionComponents.erase(std::remove(
		Engine->StoredCollisionComponents.begin(),
		Engine->StoredCollisionComponents.end(), Component));
}

void ClearScreenPrints(VoodooEngine* Engine)
{

}

void CloseApp(VoodooEngine* Engine)
{
	Engine->EngineRunning = false;
	Engine->StoredBitmapComponents.clear();
	Engine->StoredCollisionComponents.clear();
	Engine->StoredUpdateComponents.clear();
	Engine->StoredInputCallbacks.clear();
	Engine->StoredInputs.clear();
	Engine->StoredObjects.clear();
	delete Engine;
}

void ShiftBitmapToLetter(int LetterID, BitmapComponent* LetterBitmap, Text* TextParent)
{
	LetterBitmap->BitmapParams.BitmapSource.X = TextParent->LetterSpace * LetterID;
	LetterBitmap->BitmapParams.BitmapOffsetLeft.X =
		LetterBitmap->BitmapParams.BitmapSource.X - TextParent->LetterSpace;
	LetterBitmap->BitmapParams.BitmapOffsetRight.X = TextParent->LetterSpace;
}

void AssignLetterShiftByID(std::string Letter, BitmapComponent* LetterBitmap, Text* TextParent)
{
	// The numbers represent a location in the font bitmap

	if (Letter == "a")
		ShiftBitmapToLetter(1, LetterBitmap, TextParent);
	if (Letter == "b")
		ShiftBitmapToLetter(2, LetterBitmap, TextParent);
	if (Letter == "c")
		ShiftBitmapToLetter(3, LetterBitmap, TextParent);
	if (Letter == "d")
		ShiftBitmapToLetter(4, LetterBitmap, TextParent);
	if (Letter == "e")
		ShiftBitmapToLetter(5, LetterBitmap, TextParent);
	if (Letter == "f")
		ShiftBitmapToLetter(6, LetterBitmap, TextParent);
	if (Letter == "g")
		ShiftBitmapToLetter(7, LetterBitmap, TextParent);
	if (Letter == "h")
		ShiftBitmapToLetter(8, LetterBitmap, TextParent);
	if (Letter == "i")
		ShiftBitmapToLetter(9, LetterBitmap, TextParent);
	if (Letter == "j")
		ShiftBitmapToLetter(10, LetterBitmap, TextParent);
	if (Letter == "k")
		ShiftBitmapToLetter(11, LetterBitmap, TextParent);
	if (Letter == "l")
		ShiftBitmapToLetter(12, LetterBitmap, TextParent);
	if (Letter == "m")
		ShiftBitmapToLetter(13, LetterBitmap, TextParent);
	if (Letter == "n")
		ShiftBitmapToLetter(14, LetterBitmap, TextParent);
	if (Letter == "o")
		ShiftBitmapToLetter(15, LetterBitmap, TextParent);
	if (Letter == "p")
		ShiftBitmapToLetter(16, LetterBitmap, TextParent);
	if (Letter == "q")
		ShiftBitmapToLetter(17, LetterBitmap, TextParent);
	if (Letter == "r")
		ShiftBitmapToLetter(18, LetterBitmap, TextParent);
	if (Letter == "s")
		ShiftBitmapToLetter(19, LetterBitmap, TextParent);
	if (Letter == "t")
		ShiftBitmapToLetter(20, LetterBitmap, TextParent);
	if (Letter == "u")
		ShiftBitmapToLetter(21, LetterBitmap, TextParent);
	if (Letter == "v")
		ShiftBitmapToLetter(22, LetterBitmap, TextParent);
	if (Letter == "w")
		ShiftBitmapToLetter(23, LetterBitmap, TextParent);
	if (Letter == "x")
		ShiftBitmapToLetter(24, LetterBitmap, TextParent);
	if (Letter == "y")
		ShiftBitmapToLetter(25, LetterBitmap, TextParent);
	if (Letter == "z")
		ShiftBitmapToLetter(26, LetterBitmap, TextParent);
	if (Letter == ".")
		ShiftBitmapToLetter(27, LetterBitmap, TextParent);
	if (Letter == ",")
		ShiftBitmapToLetter(28, LetterBitmap, TextParent);
	if (Letter == "?")
		ShiftBitmapToLetter(29, LetterBitmap, TextParent);
	if (Letter == "!")
		ShiftBitmapToLetter(30, LetterBitmap, TextParent);
}

void CreateLetter(
	VoodooEngine* Engine, Text* TextParent, std::string LetterString, 
	SVector LetterLocation, const wchar_t* Font)
{
	BitmapComponent* CreatedLetter = new BitmapComponent();
	CreatedLetter->Bitmap = CreateNewBitmap(Engine->Renderer, Font);
	CreatedLetter->BitmapParams = SetupBitmapParams(CreatedLetter->Bitmap);
	CreatedLetter->ComponentLocation = LetterLocation;
	AssignLetterShiftByID(LetterString, CreatedLetter, TextParent);
	TextParent->StoredLetters.push_back(CreatedLetter);
}

Text* CreateText(VoodooEngine* Engine, Text* TextToCreate, ButtonParameters ButtonParams)
{
	TextToCreate = new Text();
	SVector LetterLocation = ButtonParams.ButtonLocation;
	LetterLocation.Y = ButtonParams.ButtonLocation.Y + ButtonParams.ButtonTextOffset.Y;
	float LetterOffsetX = LetterLocation.X + ButtonParams.ButtonTextOffset.X;
	for (int i = 0; i < ButtonParams.ButtonTextString.length(); i++)
	{
		// Makes room for the next letter in the text
		LetterOffsetX += TextToCreate->LetterSpace;
		LetterLocation.X = LetterOffsetX;

		// Create the next letter in the button text string 
		// (don´t create and leave white space if "_" symbol is found, 
		// but still offset the location for the next letter)
		if (ButtonParams.ButtonTextString.substr(i, 1) != "_")
		{
			CreateLetter(Engine, TextToCreate, ButtonParams.ButtonTextString.substr(i, 1),
				LetterLocation, Engine->DefaultFont);
		}
	}
	return TextToCreate;
}

void ScreenPrint(std::string DebugText, VoodooEngine* Engine)
{
	Text* CreatedText = nullptr;
	CreatedText = new Text();
	SVector LetterLocation = {0,0};
	float LetterOffsetX = LetterLocation.X;
	Engine->ScreenColumnsPrinted += 1;
	float LetterOffsetY = LetterLocation.Y += (30 * Engine->ScreenColumnsPrinted);
	for (int i = 0; i < DebugText.length(); i++)
	{
		// Makes room for the next letter in the text
		LetterOffsetX += CreatedText->LetterSpace;
		LetterLocation.X = LetterOffsetX;

		// Create the next letter in the button text string 
		// (don´t create and leave white space if "_" symbol is found, 
		// but still offset the location for the next letter)
		if (DebugText.substr(i, 1) != "_")
		{
			CreateLetter(Engine, CreatedText, DebugText.substr(i, 1),
				LetterLocation, Engine->DebugFont);
		}
		for (int i = 0; i < CreatedText->StoredLetters.size(); i++)
		{
			Engine->StoredScreenPrintTexts.push_back(CreatedText->StoredLetters[i]);
		}
	}
}

Button* CreateButton(VoodooEngine* Engine, Button* ButtonToCreate, ButtonParameters ButtonParams)
{
	// Create button bitmap and set the button location
	ButtonToCreate->ButtonBitmap = new BitmapComponent();
	ButtonToCreate->ButtonBitmap->Bitmap = 
		CreateNewBitmap(Engine->Renderer, ButtonToCreate->ButtonParams.AssetPathButtonBitmap);
	ButtonToCreate->ButtonBitmap->BitmapParams = 
		SetupBitmapParams(ButtonToCreate->ButtonBitmap->Bitmap);
	Engine->StoredEditorBitmapComponents.push_back(ButtonToCreate->ButtonBitmap);
	ButtonToCreate->ButtonBitmap->ComponentLocation = ButtonParams.ButtonLocation;
	SetBitmapSourceLocationX(ButtonToCreate->ButtonBitmap, ButtonParams.ButtonWidth);

	// Create button collider
	ButtonToCreate->ButtonCollider = new CollisionComponent();
	ButtonToCreate->ButtonCollider->CollisionRect = ButtonToCreate->ButtonBitmap->BitmapParams.BitmapSource;
	ButtonToCreate->ButtonCollider->ComponentLocation = ButtonToCreate->ButtonBitmap->ComponentLocation;
	ButtonToCreate->ButtonCollider->CollisionTag = ButtonParams.ButtonCollisionTag;
	
	if (Engine->DebugMode)
	{
		// Set purple color as default
		ButtonToCreate->ButtonCollider->CollisionRectColor = {200, 0, 255};
		ButtonToCreate->ButtonCollider->RenderCollisionRect = true;
		Engine->StoredCollisionComponents.push_back(ButtonToCreate->ButtonCollider);
	}

	// Create text for button
	ButtonToCreate->ButtonText = CreateText(Engine, ButtonToCreate->ButtonText, ButtonParams);
	// Store all created letters to be rendered
	for (int i = 0; i < ButtonToCreate->ButtonText->StoredLetters.size(); i++)
	{
		Engine->StoredEditorBitmapComponents.push_back(ButtonToCreate->ButtonText->StoredLetters[i]);
	}

	return ButtonToCreate;
}

void DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete)
{
	// Delete and free up memory from every object created with "new" related to button here 
}

void CreateMouse(VoodooEngine* Engine, SVector MouseColliderSize)
{
	// Add mouse collider used for detecting mouse "hover" (is invisible as default, when not in debug mode)
	Engine->Mouse.MouseCollider = new CollisionComponent();
	SetMouseColliderSize(Engine, MouseColliderSize);
	Engine->StoredCollisionComponents.push_back(Engine->Mouse.MouseCollider);

	Engine->Mouse.MouseBitmap = new BitmapComponent();
	Engine->Mouse.MouseBitmap->Bitmap = CreateNewBitmap(
		Engine->Renderer, L"EngineContent/Cursor/CustomMouseCursor.png");

	// Setup custom mouse cursor bitmap if found, otherwise render mouse collider instead
	if (Engine->Mouse.MouseBitmap->Bitmap)
	{
		Engine->Mouse.MouseBitmap->BitmapParams = SetupBitmapParams(Engine->Mouse.MouseBitmap->Bitmap);

		if (Engine->DebugMode)
			Engine->Mouse.MouseCollider->RenderCollisionRect = true;
	}
	else
	{
		// Auto render collision rect if no mouse cursor bitmap is found 
		// (so you still can see a visual representation of where the mouse is)
		Engine->Mouse.MouseCollider->RenderCollisionRect = true;
	}
}

void DeleteMouse(VoodooEngine* Engine)
{
	if (Engine->Mouse.MouseBitmap)
	{
		delete Engine->Mouse.MouseBitmap;
		Engine->Mouse.MouseBitmap = nullptr;
	}

	if (Engine->Mouse.MouseCollider)
	{
		DeleteCollisionComponent(Engine, Engine->Mouse.MouseCollider);
		delete Engine->Mouse.MouseCollider;
		Engine->Mouse.MouseCollider = nullptr;
	}
}

void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize)
{
	Engine->Mouse.MouseCollider->CollisionRect = ColliderSize;
}

void UpdateMouseLocation(VoodooEngine* Engine, SVector NewLocation)
{
	if (!Engine || !Engine->EngineRunning)
		return;

	Engine->Mouse.Location = NewLocation;

	if (Engine->Mouse.MouseBitmap)
		Engine->Mouse.MouseBitmap->ComponentLocation = NewLocation;

	if (Engine->Mouse.MouseCollider &&
		Engine->Mouse.MouseBitmap)
	{
		Engine->Mouse.MouseCollider->ComponentLocation =
		{Engine->Mouse.MouseBitmap->ComponentLocation.X + Engine->Mouse.MouseCollider->CollisionRectOffset.X,
		Engine->Mouse.MouseBitmap->ComponentLocation.Y + Engine->Mouse.MouseCollider->CollisionRectOffset.Y};
	}
}

void UpdateCustomMouseCursor(VoodooEngine* Engine)
{
	if (GetMessagePos())
	{
		POINT MousePosition;
		if (GetCursorPos(&MousePosition))
		{
			SVector NewMousePos = {0,0};
			NewMousePos.X = MousePosition.x;
			NewMousePos.Y = MousePosition.y;

			if (Engine)
				UpdateMouseLocation(Engine, NewMousePos);
		}
	}
}

bool HideSystemMouseCursor(UINT Message, LPARAM LParam)
{
	// Hides system mouse cursor since engine is using custom icon for cursor
	if (Message == WM_SETCURSOR &&
		LOWORD(LParam) == HTCLIENT)
	{
		SetCursor(NULL);
		return TRUE;
	}
}

void Update(VoodooEngine* Engine, float DeltaTime)
{
	for (int i = 0; i < Engine->StoredUpdateComponents.size(); i++)
	{
		Engine->StoredUpdateComponents[i]->Update(DeltaTime);
	}
}

ID2D1HwndRenderTarget* SetupRenderer(ID2D1HwndRenderTarget* RenderTarget, HWND HWind)
{
	ID2D1Factory* Factory = nullptr;
	HRESULT Result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &Factory);

	// Get user screen size
	RECT WinRect;
	GetClientRect(HWind, &WinRect);

	Result = Factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
		HWind, D2D1::SizeU(WinRect.right, WinRect.bottom)),
		&RenderTarget);

	return RenderTarget;
}

ID2D1Bitmap* CreateNewBitmap(ID2D1HwndRenderTarget* RenderTarget, 
	const wchar_t* FileName, bool Flip)
{
	ID2D1Bitmap* NewBitmap = nullptr;
	HRESULT Result;

	// Create Wic factory
	IWICImagingFactory* WicFactory = nullptr;
	Result = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&WicFactory));

	// Create decoder
	IWICBitmapDecoder* Decoder = nullptr;
	Result = WicFactory->CreateDecoderFromFilename(
		FileName,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&Decoder);

	// Failed to find file 
	if (!Decoder)
		return nullptr;

	// Create decoder frame
	IWICBitmapFrameDecode* DecoderFrame = nullptr;
	Result = Decoder->GetFrame(0, &DecoderFrame);

	// Create a converter from WIC bitmap to Direct2D bitmap
	IWICFormatConverter* WicConverter = nullptr;
	Result = WicFactory->CreateFormatConverter(&WicConverter);

	// If flip is true, then flip the image
	if (Flip)
	{
		// Setup flip rotator
		IWICBitmapFlipRotator* ImageFlip = nullptr;
		Result = WicFactory->CreateBitmapFlipRotator(&ImageFlip);
		Result = ImageFlip->Initialize(DecoderFrame, WICBitmapTransformFlipHorizontal);

		// Setup converter (for flipped image)
		Result = WicConverter->Initialize(
			ImageFlip,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0,
			WICBitmapPaletteTypeCustom);

		if (ImageFlip)
			ImageFlip->Release();
	}
	else
	{
		// Setup the converter (for NON flipped image)
		Result = WicConverter->Initialize(
			DecoderFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0,
			WICBitmapPaletteTypeCustom);
	}

	RenderTarget->CreateBitmapFromWicBitmap(
		WicConverter, nullptr, &NewBitmap);
	
	if (WicFactory)
		WicFactory->Release();
	
	if (Decoder)
		Decoder->Release();
	
	if (DecoderFrame)
		DecoderFrame->Release();
	
	if (WicConverter)
		WicConverter->Release();

	return NewBitmap;
}

SBitmap SetupBitmapParams(ID2D1Bitmap* BitmapToSetup)
{
	SBitmap BitmapSetup = {};

	// Set the bitmap source the same size as the loaded image (created bitmap)
	BitmapSetup.BitmapSource.X = BitmapToSetup->GetSize().width;
	BitmapSetup.BitmapSource.Y = BitmapToSetup->GetSize().height;

	// Since computer graphics start from left to right
	// "BitmapOffsetLeft" is not changed since default is at "0"
	BitmapSetup.BitmapOffsetRight.X = BitmapSetup.BitmapSource.X;
	BitmapSetup.BitmapOffsetRight.Y = BitmapSetup.BitmapSource.Y;

	return BitmapSetup;
}

void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int BitmapOffsetMultiplier)
{
	BitmapToUpdate->BitmapParams.BitmapSource.X = BitmapSourceWidth * BitmapOffsetMultiplier;

	BitmapToUpdate->BitmapParams.BitmapOffsetLeft.X = 
		BitmapToUpdate->BitmapParams.BitmapSource.X - BitmapSourceWidth;

	BitmapToUpdate->BitmapParams.BitmapOffsetRight.X = BitmapSourceWidth;
}

void UpdateAnimationState(AnimationParameters &AnimationParams,
	SVector &BitmapSource, SVector &BitmapOffsetLeft, SVector &BitmapOffsetRight)
{
	// Goes from top to bottom in a spritesheet depending on desired animation state
	// e.g. idle is at top row of spritesheet since that "AnimationState" number is 0, 
	// walk is a row below since "AnimationState" number is 1 etc. 
	BitmapSource.Y = AnimationParams.FrameHeight * AnimationParams.AnimationState;
	BitmapOffsetLeft.Y = BitmapSource.Y - AnimationParams.FrameHeight;
	BitmapOffsetRight.Y = AnimationParams.FrameHeight;
}

void UpdateAnimation(AnimationParameters &AnimationParams,
	SVector &BitmapSource, SVector &BitmapOffsetLeft, SVector &BitmapOffsetRight, float DeltaTime)
{
	UpdateAnimationState(AnimationParams, BitmapSource, BitmapOffsetLeft, BitmapOffsetRight);

	// Default to first frame
	int CurrentFrameLocation = AnimationParams.FrameWidth;

	// Controls speed of animation
	AnimationParams.AnimationTimer += AnimationParams.AnimationSpeed * DeltaTime;

	// Reset animation back to first frame when last frame is reached
	if (AnimationParams.AnimationTimer > AnimationParams.TotalFrames)
	{
		AnimationParams.CurrentFrame = 1;
		AnimationParams.AnimationTimer = 0;
	}

	// Only update to next animation frame if current frame is less than the total amount of frames
	// ("+1" is there to make sure the last frame is taken into account)
	if (AnimationParams.CurrentFrame < (AnimationParams.TotalFrames + 1))
	{
		// Update the spritesheet to the current frame 
		// (moves from left to right in a spritesheet)
		CurrentFrameLocation = AnimationParams.FrameWidth * AnimationParams.CurrentFrame;
		BitmapSource.X = CurrentFrameLocation;
		BitmapOffsetLeft.X = BitmapSource.X - AnimationParams.FrameWidth;
		BitmapOffsetRight.X = AnimationParams.FrameWidth;

		// Only move to next frame once animation timer has catched up
		if (AnimationParams.AnimationTimer > AnimationParams.CurrentFrame)
		{
			AnimationParams.CurrentFrame += 1;
		}
	}
}

void RenderBitmapByLayer(ID2D1HwndRenderTarget* Renderer, 
	std::vector<BitmapComponent*> StoredBitmaps, int RenderLayer)
{
	for (int i = 0; i < StoredBitmaps.size(); i++)
	{
		// Go to next if bitmap is not valid
		if (!StoredBitmaps[i]->Bitmap)
			continue;

		// Go to next bitmap if set to be hidden in game
		if (StoredBitmaps[i]->BitmapParams.HiddenInGame)
			continue;

		// Go to next if bitmap is not assigned as current render layer
		if (StoredBitmaps[i]->BitmapParams.RenderLayer != RenderLayer)
			continue;

		D2D_RECT_F DestRect =
			D2D1::RectF(
			StoredBitmaps[i]->ComponentLocation.X,
			StoredBitmaps[i]->ComponentLocation.Y,
			StoredBitmaps[i]->ComponentLocation.X + StoredBitmaps[i]->BitmapParams.BitmapOffsetRight.X,
			StoredBitmaps[i]->ComponentLocation.Y + StoredBitmaps[i]->BitmapParams.BitmapOffsetRight.Y);

		D2D_RECT_F SourceRect =
			D2D1::RectF(
			StoredBitmaps[i]->BitmapParams.BitmapOffsetLeft.X,
			StoredBitmaps[i]->BitmapParams.BitmapOffsetLeft.Y,
			StoredBitmaps[i]->BitmapParams.BitmapSource.X,
			StoredBitmaps[i]->BitmapParams.BitmapSource.Y);

		Renderer->DrawBitmap(
			StoredBitmaps[i]->Bitmap,
			DestRect,
			StoredBitmaps[i]->BitmapParams.Opacity,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			SourceRect);
	}
}

void RenderBitmaps(ID2D1HwndRenderTarget* Renderer,
	std::vector<BitmapComponent*> BitmapsToRender, int MaxNumRenderLayers)
{
	// "+1" is there to account for the last render layer 
	for (int i = 0; i < (MaxNumRenderLayers + 1); i++)
	{
		RenderBitmapByLayer(Renderer, BitmapsToRender, i);
	}
}

void RenderCustomMouseCursor(ID2D1HwndRenderTarget* Renderer, VoodooEngine* Engine)
{
	if (Engine->Mouse.MouseCollider->RenderCollisionRect)
	{
		// Render mouse collider as fallback if no custom cursor image file is found 
		RenderCollisionRectangle(Renderer, Engine->Mouse.MouseCollider);
		return;
	}
	
	D2D_RECT_F DestRect =
		D2D1::RectF(
		Engine->Mouse.MouseBitmap->ComponentLocation.X,
		Engine->Mouse.MouseBitmap->ComponentLocation.Y,
		Engine->Mouse.MouseBitmap->ComponentLocation.X + 
		Engine->Mouse.MouseBitmap->BitmapParams.BitmapOffsetRight.X,
		Engine->Mouse.MouseBitmap->ComponentLocation.Y + 
		Engine->Mouse.MouseBitmap->BitmapParams.BitmapOffsetRight.Y);

	D2D_RECT_F SourceRect =
		D2D1::RectF(
		Engine->Mouse.MouseBitmap->BitmapParams.BitmapOffsetLeft.X,
		Engine->Mouse.MouseBitmap->BitmapParams.BitmapOffsetLeft.Y,
		Engine->Mouse.MouseBitmap->BitmapParams.BitmapSource.X,
		Engine->Mouse.MouseBitmap->BitmapParams.BitmapSource.Y);

	Renderer->DrawBitmap(
		Engine->Mouse.MouseBitmap->Bitmap,
		DestRect,
		// Always render mouse cursor at full opacity
		1,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		SourceRect);
}

bool IsCollisionDetected(CollisionComponent* Sender, CollisionComponent* Target)
{
	if (Sender->ComponentLocation.X < Target->ComponentLocation.X + Target->CollisionRect.X &&
		Target->ComponentLocation.X < Sender->ComponentLocation.X + Sender->CollisionRect.X &&
		Sender->ComponentLocation.Y < Target->ComponentLocation.Y + Target->CollisionRect.Y &&
		Target->ComponentLocation.Y < Sender->ComponentLocation.Y + Sender->CollisionRect.Y)
	{
		return true;
	}

	return false;
}

void CheckForCollision(Object* CallbackOwner, CollisionComponent* Sender, CollisionComponent* Target)
{
	if (!Target)
		return;

	if (Target->NoCollision)
		return;

	bool Ignore = false;
	if (IsCollisionDetected(Sender, Target))
	{
		for (int i = 0; i < Sender->CollisionTagsToIgnore.size(); i++)
		{
			if (Target->CollisionTag == Sender->CollisionTagsToIgnore[i])
			{
				Ignore = true;
				break;
			}
		}

		if (!Ignore && 
			!Sender->IsOverlapped)
		{
			Sender->IsOverlapped = true;
			CallbackOwner->OnBeginOverlap(Sender->CollisionTag, Target->CollisionTag);
		}
	}
	else if (!Ignore && 
		Sender->IsOverlapped)
	{
		Sender->IsOverlapped = false;
		CallbackOwner->OnEndOverlap(Sender->CollisionTag, Target->CollisionTag);
	}
}

void CheckForCollisionMultiple(Object* CallbackOwner, CollisionComponent* Sender, std::vector<CollisionComponent*> Targets)
{
	for (int i = 0; i < Targets.size(); i++)
	{
		if (!Targets[i])
			return;

		if (Targets[i]->NoCollision)
			continue;

		bool Ignore = false;
		if (IsCollisionDetected(Sender, Targets[i]))
		{
			for (int j = 0; j < Sender->CollisionTagsToIgnore.size(); j++)
			{
				if (Targets[i]->CollisionTag == Sender->CollisionTagsToIgnore[j])
				{
					Ignore = true;
					break;
				}
			}

			if (!Ignore &&
				!Sender->IsOverlapped)
			{
				Sender->IsOverlapped = true;
				CallbackOwner->OnBeginOverlap(Sender->CollisionTag, Targets[i]->CollisionTag);
			}
		}
		else if (!Ignore &&
			Sender->IsOverlapped)
		{
			Sender->IsOverlapped = false;
			CallbackOwner->OnEndOverlap(Sender->CollisionTag, Targets[i]->CollisionTag);
		}
	}
}

void RenderCollisionRectangle(ID2D1HwndRenderTarget* Renderer, CollisionComponent* CollisionRectangleToRender)
{
	const D2D1_COLOR_F Color =
	{CollisionRectangleToRender->CollisionRectColor.R,
	CollisionRectangleToRender->CollisionRectColor.G,
	CollisionRectangleToRender->CollisionRectColor.B, 255};

	ID2D1SolidColorBrush* Brush;
	Renderer->CreateSolidColorBrush(Color, &Brush);

	Renderer->DrawRectangle(
		D2D1::RectF(
			CollisionRectangleToRender->ComponentLocation.X,
			CollisionRectangleToRender->ComponentLocation.Y,
			CollisionRectangleToRender->ComponentLocation.X +
			CollisionRectangleToRender->CollisionRect.X,
			CollisionRectangleToRender->ComponentLocation.Y +
			CollisionRectangleToRender->CollisionRect.Y),
			Brush);

	Brush->Release();
}

void RenderCollisionRectangles(ID2D1HwndRenderTarget* Renderer,
	std::vector<CollisionComponent*> CollisionRectsToRender)
{
	for (int i = 0; i < CollisionRectsToRender.size(); i++)
	{
		// Go to next if collision is set to none
		if (CollisionRectsToRender[i]->NoCollision)
			continue;

		// Go to next if set to not render collision rect
		if (!CollisionRectsToRender[i]->RenderCollisionRect)
			continue;

		const D2D1_COLOR_F Color = 
			{CollisionRectsToRender[i]->CollisionRectColor.R,
			CollisionRectsToRender[i]->CollisionRectColor.G,
			CollisionRectsToRender[i]->CollisionRectColor.B, 255};

		ID2D1SolidColorBrush* Brush;
		Renderer->CreateSolidColorBrush(Color, &Brush);
		
		Renderer->DrawRectangle(
			D2D1::RectF(
			CollisionRectsToRender[i]->ComponentLocation.X,
			CollisionRectsToRender[i]->ComponentLocation.Y,
			CollisionRectsToRender[i]->ComponentLocation.X +
			CollisionRectsToRender[i]->CollisionRect.X,
			CollisionRectsToRender[i]->ComponentLocation.Y +
			CollisionRectsToRender[i]->CollisionRect.Y),
			Brush);

		Brush->Release();
	}
}

SVector GetObjectLocation(Object* Object)
{
	return Object->Location;
}

void SetObjectLocation(Object* Object, SVector NewLocation)
{
	Object->Location = NewLocation;
}

void SetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component, SVector NewLocation)
{
	Component->ComponentLocation = 
		{ComponentOwner->Location.X + NewLocation.X,
		ComponentOwner->Location.Y + NewLocation.Y};
}

SVector GetComponentRelativeLocation(
	Object* ComponentOwner, TransformComponent* Component)
{
	SVector CurrentComponentLocation =
		{ComponentOwner->Location.X + Component->ComponentLocation.X,
		ComponentOwner->Location.Y + Component->ComponentLocation.Y};

	return CurrentComponentLocation;
}

bool UpdateDebugMode()
{
	bool NewDebugMode = false;
	std::fstream File("EngineConfig.txt");
	std::string String;
	if (File.is_open())
	{
		while (getline(File, String))
		{
			if (String == "DebugModeTrue")
			{
				NewDebugMode = true;
			}
			else if (String == "DebugModeFalse")
			{
				NewDebugMode = false;
			}
		}
	}
	File.close();

	return NewDebugMode;
}

bool UpdateEditorMode()
{
	bool NewEditorMode = false;
	std::fstream File("EngineConfig.txt");
	std::string String;
	if (File.is_open())
	{
		while (getline(File, String))
		{
			if (String == "EditorModeTrue")
			{
				NewEditorMode = true;
			}
			else if (String == "EditorModeFalse")
			{
				NewEditorMode = false;
			}
		}
	}
	File.close();

	return NewEditorMode;
}
