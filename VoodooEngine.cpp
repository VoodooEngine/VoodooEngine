#include "pch.h"
#include "VoodooEngine.h"

// File I/O
#include <commdlg.h>
#include <fstream>
#include <sstream>

// Create and register app window
static void CreateAppWindow(SWindowParams& WindowParams, WNDPROC InputCallbackFunction)
{
	WindowParams.WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowParams.WindowClass.lpfnWndProc = InputCallbackFunction;
	WindowParams.WindowClass.cbClsExtra = 0;
	WindowParams.WindowClass.cbWndExtra = 0;
	WindowParams.WindowClass.hInstance = nullptr;
	WindowParams.WindowClass.lpszClassName = L"Window";
	WindowParams.WindowClass.lpszMenuName = nullptr;
	WindowParams.WindowClass.hbrBackground = nullptr;
	WindowParams.WindowClass.hIcon = nullptr;
	WindowParams.WindowClass.hIconSm = nullptr;
	WindowParams.WindowClass.hCursor = nullptr;

	RegisterClassEx(&WindowParams.WindowClass);

	int WindowFullscreen = 0;
	if (WindowParams.Fullscreen)
	{
		WindowFullscreen = WS_POPUP;
	}
	else
	{
		WindowFullscreen = WS_OVERLAPPEDWINDOW;
	}

	WindowParams.HWind = CreateWindow(
		WindowParams.WindowClass.lpszClassName,
		WindowParams.WindowTitle,
		WindowFullscreen,
		0, 0,
		WindowParams.ScreenResolutionWidth,
		WindowParams.ScreenResolutionHeight,
		nullptr,
		nullptr,
		WindowParams.WindowClass.hInstance,
		nullptr);
}

// Updates any changes made to the app window (e.g. dragging the window)
static void UpdateAppWindow()
{
	MSG MSGMessage;
	MSGMessage.message = WM_NULL;
	while (PeekMessage(&MSGMessage, nullptr, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&MSGMessage);
	}
}

// This will set a custom assigned icon of the app window title/task bar
// (will default to windows default app icon if no valid custom icon is found)
static void SetCustomAppIcon(VoodooEngine* Engine)
{
	// Setup the app icon path
	const wchar_t* IconPath;
	if (Engine->EditorMode)
	{
		IconPath = L"EngineContent/Ico/EngineIcon.ico";
	}
	else
	{
		IconPath = L"GameIcon.ico";
	}

	// Load custom app icon from file (ico file format)
	HANDLE CustomAppIcon = LoadImage(
		0, IconPath,
		IMAGE_ICON, 0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE);

	// Assign the custom icon as the official icon of the app 
	// (if no icon is found, then the engine will use default desktop icon)
	if (CustomAppIcon)
	{
		SendMessage(Engine->Window.HWind, WM_SETICON, ICON_SMALL, (LPARAM)CustomAppIcon);
		SendMessage(Engine->Window.HWind, WM_SETICON, ICON_BIG, (LPARAM)CustomAppIcon);
		
		SendMessage(GetWindow(
			Engine->Window.HWind, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)CustomAppIcon);
		SendMessage(GetWindow(
			Engine->Window.HWind, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)CustomAppIcon);
	}
}
ID2D1HwndRenderTarget* SetupRenderer(ID2D1HwndRenderTarget* Renderer, HWND HWind)
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
		&Renderer);

	Factory->Release();

	return Renderer;
}

// Setup default brushes used by various objects that needs a brush 
// (so we don't create new brushes for every object)
static void SetupDefaultBrushes(VoodooEngine* Engine)
{
	Engine->Renderer->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&Engine->BlackBrush);

	Engine->Renderer->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&Engine->WhiteBrush);
}

static UINT64 VoodooEngineGetTicks(VoodooEngine* Engine)
{
	QueryPerformanceCounter(&Engine->CurrentTicks);
	return (UINT64)(((Engine->CurrentTicks.QuadPart - 
		Engine->StartTicks.QuadPart) * 1000) / Engine->TicksPerSecond.QuadPart);
}

static void UpdateFrameRate(VoodooEngine* Engine)
{
	Engine->TimeToWait = Engine->FrameTargetTime - (VoodooEngineGetTicks(Engine) - Engine->PreviousFrameTime);
	if (Engine->TimeToWait > 0 &&
		Engine->TimeToWait <= Engine->FrameTargetTime)
	{
		Sleep(Engine->TimeToWait);
	}
	Engine->DeltaTime = (VoodooEngineGetTicks(Engine) - Engine->PreviousFrameTime) / 1000.0;
	Engine->PreviousFrameTime = VoodooEngineGetTicks(Engine);
}

void SetFPSLimit(VoodooEngine* Engine, float FPSLimit)
{
	Engine->FPS = FPSLimit;
	Engine->FrameTargetTime = (1000 / Engine->FPS);
}

void BroadcastInput(std::vector<IInput*> StoredCallbacks, int Input, bool Pressed)
{
	for (int i = 0; i < StoredCallbacks.size(); i++)
	{
		StoredCallbacks[i]->OnInputBroadcast(Input, Pressed);
	}
}

// Check if specific input is being pressed/released, 
// returns false (not pressed) as default if no input is found
bool CheckInputPressed(std::map<int, bool> StoredInputs, int InputToCheck)
{
	int KeyToCheck = InputToCheck;
	auto Iterator = StoredInputs.find(KeyToCheck);

	if (Iterator != StoredInputs.end())
	{
		return Iterator->second;
	}

	return false;
}

void RemoveBitmapComponent(BitmapComponent* Component, VoodooEngine* Engine)
{
	Engine->StoredBitmapComponents.erase(std::remove(
		Engine->StoredBitmapComponents.begin(),
		Engine->StoredBitmapComponents.end(), Component));
}

void RemoveCollisionComponent(CollisionComponent* Component, VoodooEngine* Engine)
{
	Engine->StoredCollisionComponents.erase(std::remove(
		Engine->StoredCollisionComponents.begin(),
		Engine->StoredCollisionComponents.end(), Component));
}

void RemoveUpdateComponent(UpdateComponent* Component, VoodooEngine* Engine)
{
	Engine->StoredUpdateComponents.erase(std::remove(
		Engine->StoredUpdateComponents.begin(),
		Engine->StoredUpdateComponents.end(), Component));
}

void RemoveInputCallback(IInput* Component, VoodooEngine* Engine)
{
	Engine->InterfaceObjects_InputCallback.erase(std::remove(
		Engine->InterfaceObjects_InputCallback.begin(),
		Engine->InterfaceObjects_InputCallback.end(), Component));
}

void ShiftBitmapToLetter(int LetterID, BitmapComponent* LetterBitmap, VoodooEngine* Engine)
{
	LetterBitmap->BitmapParams.BitmapSource.X = Engine->LetterSpace * LetterID;
	LetterBitmap->BitmapParams.BitmapOffsetLeft.X =
		LetterBitmap->BitmapParams.BitmapSource.X - Engine->LetterSpace;
	LetterBitmap->BitmapParams.BitmapOffsetRight.X = Engine->LetterSpace;
}

void AssignLetterShiftByID(std::string Letter, BitmapComponent* LetterBitmap, VoodooEngine* Engine)
{
	// The numbers represent a location in the font bitmap

	if (Letter == "a")
		ShiftBitmapToLetter(1, LetterBitmap, Engine);
	if (Letter == "b")
		ShiftBitmapToLetter(2, LetterBitmap, Engine);
	if (Letter == "c")
		ShiftBitmapToLetter(3, LetterBitmap, Engine);
	if (Letter == "d")
		ShiftBitmapToLetter(4, LetterBitmap, Engine);
	if (Letter == "e")
		ShiftBitmapToLetter(5, LetterBitmap, Engine);
	if (Letter == "f")
		ShiftBitmapToLetter(6, LetterBitmap, Engine);
	if (Letter == "g")
		ShiftBitmapToLetter(7, LetterBitmap, Engine);
	if (Letter == "h")
		ShiftBitmapToLetter(8, LetterBitmap, Engine);
	if (Letter == "i")
		ShiftBitmapToLetter(9, LetterBitmap, Engine);
	if (Letter == "j")
		ShiftBitmapToLetter(10, LetterBitmap, Engine);
	if (Letter == "k")
		ShiftBitmapToLetter(11, LetterBitmap, Engine);
	if (Letter == "l")
		ShiftBitmapToLetter(12, LetterBitmap, Engine);
	if (Letter == "m")
		ShiftBitmapToLetter(13, LetterBitmap, Engine);
	if (Letter == "n")
		ShiftBitmapToLetter(14, LetterBitmap, Engine);
	if (Letter == "o")
		ShiftBitmapToLetter(15, LetterBitmap, Engine);
	if (Letter == "p")
		ShiftBitmapToLetter(16, LetterBitmap, Engine);
	if (Letter == "q")
		ShiftBitmapToLetter(17, LetterBitmap, Engine);
	if (Letter == "r")
		ShiftBitmapToLetter(18, LetterBitmap, Engine);
	if (Letter == "s")
		ShiftBitmapToLetter(19, LetterBitmap, Engine);
	if (Letter == "t")
		ShiftBitmapToLetter(20, LetterBitmap, Engine);
	if (Letter == "u")
		ShiftBitmapToLetter(21, LetterBitmap, Engine);
	if (Letter == "v")
		ShiftBitmapToLetter(22, LetterBitmap, Engine);
	if (Letter == "w")
		ShiftBitmapToLetter(23, LetterBitmap, Engine);
	if (Letter == "x")
		ShiftBitmapToLetter(24, LetterBitmap, Engine);
	if (Letter == "y")
		ShiftBitmapToLetter(25, LetterBitmap, Engine);
	if (Letter == "z")
		ShiftBitmapToLetter(26, LetterBitmap, Engine);
	if (Letter == ".")
		ShiftBitmapToLetter(27, LetterBitmap, Engine);
	if (Letter == ",")
		ShiftBitmapToLetter(28, LetterBitmap, Engine);
	if (Letter == "?")
		ShiftBitmapToLetter(29, LetterBitmap, Engine);
	if (Letter == "!")
		ShiftBitmapToLetter(30, LetterBitmap, Engine);
}

BitmapComponent* CreateLetter(
	VoodooEngine* Engine, std::string LetterString, 
	SVector LetterLocation, const wchar_t* Font)
{
	BitmapComponent* CreatedLetter = new BitmapComponent();
	CreatedLetter->Bitmap = SetupBitmap(CreatedLetter->Bitmap, Font, Engine->Renderer);
	SetupBitmapComponent(CreatedLetter, CreatedLetter->Bitmap);
	CreatedLetter->ComponentLocation = LetterLocation;
	AssignLetterShiftByID(LetterString, CreatedLetter, Engine);

	return CreatedLetter;
}

void CreateUITextFormat(VoodooEngine* Engine)
{
	IDWriteFactory* IDWriteFactory = nullptr;

	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&IDWriteFactory));

	IDWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f * 96.0f / 72.0f,
		L"en-US",
		&Engine->TextFormat);

	IDWriteFactory->Release();
}

void CreateText(VoodooEngine* Engine, Button* ButtonReference, SButtonParameters ButtonParams)
{
	SEditorAssetList FontAssetPath;

	SVector LetterLocation = ButtonParams.ButtonLocation;
	LetterLocation.Y = ButtonParams.ButtonLocation.Y + ButtonParams.ButtonTextOffset.Y;
	float LetterOffsetX = LetterLocation.X + ButtonParams.ButtonTextOffset.X;
	for (int i = 0; i < ButtonParams.ButtonTextString.length(); i++)
	{
		// Makes room for the next letter in the text
		LetterOffsetX += Engine->LetterSpace;
		LetterLocation.X = LetterOffsetX;

		// Create the next letter in the button text string 
		// (don´t create and leave white space if "_" symbol is found, 
		// but still offset the location for the next letter)
		if (ButtonParams.ButtonTextString.substr(i, 1) != "_")
		{
			BitmapComponent* NewLetter = 
				CreateLetter(Engine, ButtonParams.ButtonTextString.substr(i, 1),
				LetterLocation, FontAssetPath.DefaultFont);

			ButtonReference->ButtonText.push_back(NewLetter);
			Engine->StoredButtonTexts.push_back(NewLetter);
		}
	}
}

void RenderUITextsRenderLayer(VoodooEngine* Engine)
{
	D2D1_RECT_F OriginTextLocation = D2D1::RectF(1680.f, 110.f, 2000.f, 110.f);
	float OffsetLocationY = 50;
	for (int i = 0; i < Engine->StoredRenderLayerUITexts.size(); i++)
	{
		auto Iterator = Engine->StoredRenderLayerUITexts.find(i);
		if (Iterator->second.HideText)
		{
			return;
		}

		if (Iterator->second.TextRenderType == ETextBrushRenderType::RenderBlackBrush)
		{
			Engine->Renderer->DrawText(
				Iterator->second.Text,
				wcslen(Iterator->second.Text),
				Engine->TextFormat,
				OriginTextLocation,
				Engine->BlackBrush);
		}
		else if (Iterator->second.TextRenderType == ETextBrushRenderType::RenderWhiteBrush)
		{
			Engine->Renderer->DrawText(
				Iterator->second.Text,
				wcslen(Iterator->second.Text),
				Engine->TextFormat,
				OriginTextLocation,
				Engine->WhiteBrush);
		}

		OriginTextLocation.top += OffsetLocationY;
		OriginTextLocation.bottom += OffsetLocationY;
	}
}

void ScreenPrint(std::string DebugText, VoodooEngine* Engine)
{
	if (!Engine)
	{
		return;
	}

	SEditorAssetList FontAssetPath;

	float OriginPositionY = 100;
	SVector LetterLocation = { 0, OriginPositionY };
	float LetterOffsetX = LetterLocation.X;
	Engine->ScreenPrintTextColumnsPrinted += 1;
	float OffsetAmount = 30;
	float LetterOffsetY = LetterLocation.Y += (OffsetAmount * Engine->ScreenPrintTextColumnsPrinted);
	for (int i = 0; i < DebugText.length(); i++)
	{
		// Makes room for the next letter in the text
		LetterOffsetX += Engine->LetterSpace;
		LetterLocation.X = LetterOffsetX;

		// Create the next letter in the button text string 
		// (don´t create and leave white space if "_" symbol is found, 
		// but still offset the location for the next letter)
		if (DebugText.substr(i, 1) != "_")
		{
			BitmapComponent* NewLetter = CreateLetter(Engine, DebugText.substr(i, 1),
				LetterLocation, FontAssetPath.DebugFont);

			Engine->StoredScreenPrintTexts.push_back(NewLetter);
		}
	}
}

Button* CreateButton(
	VoodooEngine* Engine,
	Button* ButtonToCreate,
	int ButtonID,
	EButtonType ButtonType,
	std::string ButtonText,
	SVector ButtonLocation, const wchar_t* AssetPath)
{
	// Create button class and setup button parameters
	ButtonToCreate = new Button();
	ButtonToCreate->ButtonParams.ButtonCollisionTag = ButtonID;
	ButtonToCreate->ButtonParams.ButtonType = ButtonType;
	ButtonToCreate->ButtonParams.ButtonTextString = ButtonText;
	ButtonToCreate->ButtonParams.ButtonLocation = ButtonLocation;
	ButtonToCreate->ButtonParams.AssetPathButtonBitmap = AssetPath;

	// Create button bitmap and setup bitmap parameters
	ButtonToCreate->ButtonBitmap.Bitmap = 
		SetupBitmap(
		ButtonToCreate->ButtonBitmap.Bitmap, 
		ButtonToCreate->ButtonParams.AssetPathButtonBitmap,
		Engine->Renderer);
	SetupBitmapComponent(
		&ButtonToCreate->ButtonBitmap, ButtonToCreate->ButtonBitmap.Bitmap);
	ButtonToCreate->ButtonBitmap.ComponentLocation = ButtonToCreate->ButtonParams.ButtonLocation;	

	// Setup bitmap source based on button type
	SVector BitmapVector2D = { 0, 0 };
	switch (ButtonToCreate->ButtonParams.ButtonType)
	{
	case OneSided:
		BitmapVector2D.X = ButtonToCreate->ButtonBitmap.Bitmap->GetSize().width;
		BitmapVector2D.Y = ButtonToCreate->ButtonBitmap.Bitmap->GetSize().height;
		SetBitmapSourceLocationX(&ButtonToCreate->ButtonBitmap, BitmapVector2D.X);
		break;
	case TwoSided:
		BitmapVector2D.X = ButtonToCreate->ButtonBitmap.Bitmap->GetSize().width / 2;
		BitmapVector2D.Y = ButtonToCreate->ButtonBitmap.Bitmap->GetSize().height;
		SetBitmapSourceLocationX(&ButtonToCreate->ButtonBitmap, BitmapVector2D.X);
		break;
	case AssetButtonThumbnail:
		// By default the bitmap source gets set by the dimensions of the bitmap
		// but for the level editor asset thumbnail we want to crop the image to fit the asset button
		BitmapVector2D = Engine->AssetButtonThumbnailDimensions;
		SetBitmapSourceLocationX(
			&ButtonToCreate->ButtonBitmap, BitmapVector2D.X);
		SetBitmapSourceLocationY(
			&ButtonToCreate->ButtonBitmap, BitmapVector2D.Y);
		break;
	}
	Engine->StoredButtonBitmapComponents.push_back(&ButtonToCreate->ButtonBitmap);

	// If asset button type, create asset background bitmap
	if (ButtonType == AssetButtonThumbnail)
	{
		ButtonToCreate->AdditionalBackgroundBitmap.Bitmap = 
			SetupBitmap(ButtonToCreate->AdditionalBackgroundBitmap.Bitmap,
			L"EngineContent/LevelEditor/AssetButtonBase.png", Engine->Renderer);
		
		SetupBitmapComponent(
			&ButtonToCreate->AdditionalBackgroundBitmap, 
			ButtonToCreate->AdditionalBackgroundBitmap.Bitmap);
		
		ButtonToCreate->AdditionalBackgroundBitmap.ComponentLocation = 
			ButtonToCreate->ButtonParams.ButtonLocation;
		Engine->StoredButtonBitmapComponents.push_back(&ButtonToCreate->AdditionalBackgroundBitmap);
	}

	// Create button collider
	ButtonToCreate->ButtonCollider.CollisionRect = BitmapVector2D;
	ButtonToCreate->ButtonCollider.ComponentLocation = ButtonToCreate->ButtonBitmap.ComponentLocation;
	ButtonToCreate->ButtonCollider.CollisionTag = ButtonToCreate->ButtonParams.ButtonCollisionTag;
	// Only render collision rect if in debug mode
	if (Engine->DebugMode)
	{
		ButtonToCreate->ButtonCollider.CollisionRectColor = Engine->EditorCollisionRectColor;
		ButtonToCreate->ButtonCollider.RenderCollisionRect = true;
	}
	Engine->StoredEditorCollisionComponents.push_back(&ButtonToCreate->ButtonCollider);
	
	// Create text for button if button desired text is not empty
	if (ButtonToCreate->ButtonParams.ButtonTextString != "")
	{
		CreateText(Engine, ButtonToCreate, ButtonToCreate->ButtonParams);
	}

	return ButtonToCreate;
}

Button* DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete)
{	
	if (!ButtonToDelete)
	{
		return nullptr;
	}
	
	Engine->StoredButtonBitmapComponents.erase(std::remove(
		Engine->StoredButtonBitmapComponents.begin(),
		Engine->StoredButtonBitmapComponents.end(), &ButtonToDelete->ButtonBitmap));

	Engine->StoredEditorCollisionComponents.erase(std::remove(
		Engine->StoredEditorCollisionComponents.begin(),
		Engine->StoredEditorCollisionComponents.end(), &ButtonToDelete->ButtonCollider));

	while (!ButtonToDelete->ButtonText.empty())
	{
		for (int ButtonToDeleteIndex = 0; 
			ButtonToDeleteIndex < ButtonToDelete->ButtonText.size(); 
			ButtonToDeleteIndex++)
		{
			BitmapComponent* CurrentButtonText = 
				ButtonToDelete->ButtonText[ButtonToDeleteIndex];

			for (int EngineStoredButtonsIndex = 0;
				EngineStoredButtonsIndex < Engine->StoredButtonTexts.size(); 
				EngineStoredButtonsIndex++)
			{
				if (Engine->StoredButtonTexts[EngineStoredButtonsIndex] == 
					CurrentButtonText)
				{
					BitmapComponent* TextToDelete =
						Engine->StoredButtonTexts[EngineStoredButtonsIndex];

					Engine->StoredButtonTexts.erase(std::remove(
						Engine->StoredButtonTexts.begin(),
						Engine->StoredButtonTexts.end(), TextToDelete));

					ButtonToDelete->ButtonText.erase(std::remove(
						ButtonToDelete->ButtonText.begin(),
						ButtonToDelete->ButtonText.end(), CurrentButtonText));

					delete TextToDelete;
				}
			}
		}
	}

	delete ButtonToDelete;
	return nullptr;
}

void SetButtonText(Button* ButtonTextToUpdate, EButtonState ButtonState)
{
	for (int i = 0; i < ButtonTextToUpdate->ButtonText.size(); i++)
	{
		switch (ButtonState)
		{
		case Default:
			ButtonTextToUpdate->ButtonText[i]->BitmapParams.HiddenInGame = false;
			break;
		case Disabled:
			ButtonTextToUpdate->ButtonText[i]->BitmapParams.HiddenInGame = false;
			break;
		case Hidden:
			ButtonTextToUpdate->ButtonText[i]->BitmapParams.HiddenInGame = true;
			break;
		}
	}
}

void SetButtonState(
	Button* ButtonToUpdate, EButtonState NewButtonState, bool KeepBitmapOffsetUnchanged)
{
	if (ButtonToUpdate == nullptr)
	{
		return;
	}

	switch (NewButtonState)
	{
	case Default:
		ButtonToUpdate->ButtonBitmap.BitmapParams.HiddenInGame = false;
		ButtonToUpdate->AdditionalBackgroundBitmap.BitmapParams.HiddenInGame = false;
		ButtonToUpdate->ButtonCollider.NoCollision = false;
		SetButtonText(ButtonToUpdate, NewButtonState);
		// Offset the bitmap source to the "default" location
		if (!KeepBitmapOffsetUnchanged)
		{
			SetBitmapSourceLocationX(
				&ButtonToUpdate->ButtonBitmap,
				ButtonToUpdate->ButtonBitmap.Bitmap->GetSize().width / 2,
				1);
		}
		break;
	case Disabled:
		ButtonToUpdate->ButtonBitmap.BitmapParams.HiddenInGame = false;
		ButtonToUpdate->AdditionalBackgroundBitmap.BitmapParams.HiddenInGame = false;
		ButtonToUpdate->ButtonCollider.NoCollision = true;
		SetButtonText(ButtonToUpdate, NewButtonState);
		// Offset the bitmap source to the "disabled" location
		if (!KeepBitmapOffsetUnchanged)
		{
			SetBitmapSourceLocationX(
				&ButtonToUpdate->ButtonBitmap,
				ButtonToUpdate->ButtonBitmap.Bitmap->GetSize().width / 2,
				2);
		}
		break;
	case Hidden:
		ButtonToUpdate->ButtonBitmap.BitmapParams.HiddenInGame = true;
		ButtonToUpdate->AdditionalBackgroundBitmap.BitmapParams.HiddenInGame = true;
		ButtonToUpdate->ButtonCollider.NoCollision = true;
		SetButtonText(ButtonToUpdate, NewButtonState);
		break;
	}
}

void CreateMouse(VoodooEngine* Engine, SVector MouseColliderSize)
{
	// Add mouse collider used for detecting mouse "hover" (is invisible as default, when not in debug mode)
	SetMouseColliderSize(Engine, MouseColliderSize);
	Engine->StoredEditorCollisionComponents.push_back(&Engine->Mouse.MouseCollider);

	if (Engine->Mouse.MouseBitmap.Bitmap =
		SetupBitmap(Engine->Mouse.MouseBitmap.Bitmap,
		L"EngineContent/Cursor/CustomMouseCursor.png", Engine->Renderer))
	{
		SetupBitmapComponent(&Engine->Mouse.MouseBitmap, Engine->Mouse.MouseBitmap.Bitmap);
	}

	// Setup custom mouse cursor bitmap if found, otherwise render mouse collider instead
	if (Engine->Mouse.MouseBitmap.Bitmap)
	{
		if (Engine->DebugMode)
		{ 
			Engine->Mouse.MouseCollider.RenderCollisionRect = true;
		}
	}
	else
	{
		// Auto render collision rect if no mouse cursor bitmap is found 
		// (so you still can see a visual representation of where the mouse is)
		Engine->Mouse.MouseCollider.RenderCollisionRect = true;
	}
}

void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize)
{
	Engine->Mouse.MouseCollider.CollisionRect = ColliderSize;
}

void SetCustomMouseCursorLocation(VoodooEngine* Engine, SVector NewLocation)
{
	if (!Engine || 
		!Engine->EngineRunning)
	{
		return;
	}

	Engine->Mouse.Location = NewLocation;

	Engine->Mouse.MouseBitmap.ComponentLocation = NewLocation;

	Engine->Mouse.MouseCollider.ComponentLocation =
		{Engine->Mouse.MouseBitmap.ComponentLocation.X + Engine->Mouse.MouseCollider.CollisionRectOffset.X,
		Engine->Mouse.MouseBitmap.ComponentLocation.Y + Engine->Mouse.MouseCollider.CollisionRectOffset.Y};
}

void UpdateCustomMouseCursorLocation(VoodooEngine* Engine)
{
	POINT MousePositionPoint;
	GetCursorPos(&MousePositionPoint);
	SVector MousePosition = { 0, 0 };
	MousePosition.X = MousePositionPoint.x;
	MousePosition.Y = MousePositionPoint.y;
	
	SetCustomMouseCursorLocation(Engine, MousePosition);
}

void SetMouseState(bool Show, VoodooEngine* Engine)
{
	if (Show)
	{
		Engine->Mouse.MouseCollider.NoCollision = false;
		Engine->Mouse.MouseBitmap.BitmapParams.HiddenInGame = false;

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
		Engine->Mouse.MouseBitmap.BitmapParams.HiddenInGame = true;
		Engine->Mouse.MouseCollider.RenderCollisionRect = false;

		Engine->Mouse.MouseCollider.CollisionRectColor = { 0.2, 0.5, 0 };
	}
}

void Update(VoodooEngine* Engine)
{
	UpdateFrameRate(Engine);
	UpdateAppWindow();
	UpdateCustomMouseCursorLocation(Engine);

	if (Engine->EditorMode)
	{
		for (int i = 0; i < Engine->StoredEditorUpdateComponents.size(); i++)
		{
			Engine->StoredEditorUpdateComponents[i]->Update(Engine->DeltaTime);
		}
	}

	if (Engine->GameRunning)
	{
		for (int i = 0; i < Engine->StoredUpdateComponents.size(); i++)
		{
			Engine->StoredUpdateComponents[i]->Update(Engine->DeltaTime);
		}
	}
}

ID2D1Bitmap* BitmapCreationSetup(
	ID2D1Bitmap* BitmapToCreate, const wchar_t* FileName, ID2D1HwndRenderTarget* Renderer, bool Flip)
{
	// If bitmap is already created, 
	// then release previous bitmap to avoid memory leak before making it nullptr
	if (BitmapToCreate)
	{
		BitmapToCreate->Release();
		BitmapToCreate = nullptr;
	}

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
	{ 
		return nullptr;
	}

	// Create decoder frame
	IWICBitmapFrameDecode* DecoderFrame = nullptr;
	Result = Decoder->GetFrame(0, &DecoderFrame);

	// Create a converter from WIC bitmap to Direct2D bitmap
	IWICFormatConverter* WicConverter = nullptr;
	Result = WicFactory->CreateFormatConverter(&WicConverter);

	// Flip image if true, otherwise go with default image
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
		{ 
			ImageFlip->Release();
		}
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

	Renderer->CreateBitmapFromWicBitmap(
		WicConverter, nullptr, &BitmapToCreate);

	if (WicFactory)
	{ 
		WicFactory->Release();
	}
	if (Decoder)
	{ 
		Decoder->Release();
	}
	if (DecoderFrame)
	{ 
		DecoderFrame->Release();
	}
	if (WicConverter)
	{
		WicConverter->Release();
	}

	return BitmapToCreate;
}

ID2D1Bitmap* SetupBitmap(
	ID2D1Bitmap* BitmapToSetup, const wchar_t* FileName, ID2D1HwndRenderTarget * Renderer)
{
	 return BitmapCreationSetup(BitmapToSetup, FileName, Renderer, false);
}

void SetupBitmapComponent(
	BitmapComponent* BitmapComponentToSetup,
	ID2D1Bitmap* TextureAtlas,
	SVector TextureAtlasWidthHeight,
	SVector TextureAtlasOffsetMultiplierWidthHeight,
	bool UseEntireTextureAtlasAsBitmapSource)
{	
	BitmapComponentToSetup->Bitmap = TextureAtlas;
	
	if (UseEntireTextureAtlasAsBitmapSource)
	{
		// Set the bitmap source the same size as the entire texture altas
		// (this is used for when there is a single texture instead of multiple "slots" in the texture atlas)
		BitmapComponentToSetup->BitmapParams.BitmapSource.X = 
			TextureAtlas->GetSize().width;
		BitmapComponentToSetup->BitmapParams.BitmapSource.Y =
			TextureAtlas->GetSize().height;

		// Since computer graphics start from left to right
		// "BitmapOffsetLeft" is not changed since default is at "0"
		BitmapComponentToSetup->BitmapParams.BitmapOffsetRight.X =
			BitmapComponentToSetup->BitmapParams.BitmapSource.X;
		BitmapComponentToSetup->BitmapParams.BitmapOffsetRight.Y =
			BitmapComponentToSetup->BitmapParams.BitmapSource.Y;
	}
	else
	{
		// Set the bitmap source the same size as the desired texture atlas "slot" width and height
		BitmapComponentToSetup->BitmapParams.BitmapSource.X = TextureAtlasWidthHeight.X;
		BitmapComponentToSetup->BitmapParams.BitmapSource.Y = TextureAtlasWidthHeight.Y;

		// Offset the bitmap source to the desired location of the texture atlas
		SetBitmapSourceLocationX(
			BitmapComponentToSetup, 
			TextureAtlasWidthHeight.X, 
			TextureAtlasOffsetMultiplierWidthHeight.X);
		SetBitmapSourceLocationY(
			BitmapComponentToSetup,
			TextureAtlasWidthHeight.Y,
			TextureAtlasOffsetMultiplierWidthHeight.Y);
	}
}

ID2D1Bitmap* FlipBitmap(
	ID2D1Bitmap* BitmapToFlip, const wchar_t* FileName, ID2D1HwndRenderTarget* Renderer, bool Flip)
{
	return BitmapCreationSetup(BitmapToFlip, FileName, Renderer, Flip);
}

void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int BitmapOffsetMultiplier)
{
	BitmapToUpdate->BitmapParams.BitmapSource.X = BitmapSourceWidth * BitmapOffsetMultiplier;

	BitmapToUpdate->BitmapParams.BitmapOffsetLeft.X = 
		BitmapToUpdate->BitmapParams.BitmapSource.X - BitmapSourceWidth;

	BitmapToUpdate->BitmapParams.BitmapOffsetRight.X = BitmapSourceWidth;
}

void SetBitmapSourceLocationY(
	BitmapComponent* BitmapToUpdate, int BitmapSourceHeight, int BitmapOffsetMultiplier)
{
	BitmapToUpdate->BitmapParams.BitmapSource.Y = BitmapSourceHeight * BitmapOffsetMultiplier;

	BitmapToUpdate->BitmapParams.BitmapOffsetLeft.Y =
		BitmapToUpdate->BitmapParams.BitmapSource.Y - BitmapSourceHeight;

	BitmapToUpdate->BitmapParams.BitmapOffsetRight.Y = BitmapSourceHeight;
}

void SetAnimationState(SAnimationParameters &AnimationParams,
	SVector &BitmapSource, SVector &BitmapOffsetLeft, SVector &BitmapOffsetRight)
{
	// Goes from top to bottom in a spritesheet depending on desired animation state
	// e.g. idle is at top row of spritesheet since that "AnimationState" number is 0, 
	// walk is a row below since "AnimationState" number is 1 etc. 
	BitmapSource.Y = AnimationParams.FrameHeight * AnimationParams.AnimationState;
	BitmapOffsetLeft.Y = BitmapSource.Y - AnimationParams.FrameHeight;
	BitmapOffsetRight.Y = AnimationParams.FrameHeight;
}

void UpdateAnimation(SAnimationParameters &AnimationParams,
	SVector &BitmapSource, SVector &BitmapOffsetLeft, SVector &BitmapOffsetRight, float DeltaTime)
{
	SetAnimationState(AnimationParams, BitmapSource, BitmapOffsetLeft, BitmapOffsetRight);

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

void InitAnimationFirstFrame(SAnimationParameters &AnimationParams, 
	SVector &BitmapSource, SVector &BitmapOffsetLeft, SVector &BitmapOffsetRight)
{
	UpdateAnimation(AnimationParams, BitmapSource, BitmapOffsetLeft, BitmapOffsetRight, 1);
}

void RenderBitmap(ID2D1HwndRenderTarget* Renderer, BitmapComponent* BitmapToRender)
{
	if (!BitmapToRender)
	{
		return;
	}

	D2D_RECT_F DestRect =
		D2D1::RectF(
		BitmapToRender->ComponentLocation.X,
		BitmapToRender->ComponentLocation.Y,
		BitmapToRender->ComponentLocation.X + BitmapToRender->BitmapParams.BitmapOffsetRight.X,
		BitmapToRender->ComponentLocation.Y + BitmapToRender->BitmapParams.BitmapOffsetRight.Y);

	D2D_RECT_F SourceRect =
		D2D1::RectF(
		BitmapToRender->BitmapParams.BitmapOffsetLeft.X,
		BitmapToRender->BitmapParams.BitmapOffsetLeft.Y,
		BitmapToRender->BitmapParams.BitmapSource.X,
		BitmapToRender->BitmapParams.BitmapSource.Y);

	Renderer->DrawBitmap(
		BitmapToRender->Bitmap,
		DestRect,
		BitmapToRender->BitmapParams.Opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		SourceRect);
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

		RenderBitmap(Renderer, StoredBitmaps[i]);
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
	// Render mouse collider as fallback if no custom cursor image file is found or in debug mode
	if (Engine->Mouse.MouseBitmap.Bitmap == nullptr || 
		Engine->DebugMode)
	{
		// Skip rendereing altogether if set to not render
		if (!Engine->Mouse.MouseCollider.RenderCollisionRect)
		{
			return;
		}

		D2D1_RECT_F Rect = D2D1::RectF(
			Engine->Mouse.MouseCollider.ComponentLocation.X,
			Engine->Mouse.MouseCollider.ComponentLocation.Y,
			Engine->Mouse.MouseCollider.ComponentLocation.X +
			Engine->Mouse.MouseCollider.CollisionRect.X,
			Engine->Mouse.MouseCollider.ComponentLocation.Y +
			Engine->Mouse.MouseCollider.CollisionRect.Y);

		Renderer->DrawRectangle(Rect, Engine->WhiteBrush);
	}
	
	if (Engine->Mouse.MouseBitmap.Bitmap == nullptr ||
		Engine->Mouse.MouseBitmap.BitmapParams.HiddenInGame)
	{
		return;
	}

	D2D_RECT_F DestRect =
		D2D1::RectF(
		Engine->Mouse.MouseBitmap.ComponentLocation.X,
		Engine->Mouse.MouseBitmap.ComponentLocation.Y,
		Engine->Mouse.MouseBitmap.ComponentLocation.X + 
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetRight.X,
		Engine->Mouse.MouseBitmap.ComponentLocation.Y + 
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetRight.Y);

	D2D_RECT_F SourceRect =
		D2D1::RectF(
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetLeft.X,
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetLeft.Y,
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapSource.X,
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapSource.Y);

	Renderer->DrawBitmap(
		Engine->Mouse.MouseBitmap.Bitmap,
		DestRect,
		// Always render mouse cursor at full opacity
		1,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		SourceRect);
}

bool IsCollisionDetected(CollisionComponent* Sender, CollisionComponent* Target)
{
	if (Sender->NoCollision || 
		Target->NoCollision ||
		Sender == Target)
	{
		return false;
	}

	for (int i = 0; i < Sender->CollisionTagsToIgnore.size(); i++)
	{
		if (Sender->CollisionTagsToIgnore[i] == Target->CollisionTag)
		{
			return false;
		}
	}

	if (Sender->ComponentLocation.X < Target->ComponentLocation.X + Target->CollisionRect.X &&
		Target->ComponentLocation.X < Sender->ComponentLocation.X + Sender->CollisionRect.X &&
		Sender->ComponentLocation.Y < Target->ComponentLocation.Y + Target->CollisionRect.Y &&
		Target->ComponentLocation.Y < Sender->ComponentLocation.Y + Sender->CollisionRect.Y)
	{
		return true;
	}

	return false;
}

void BroadcastCollision(Object* CallbackOwner, CollisionComponent* Sender, CollisionComponent* Target)
{
	if (!Sender)
	{ 
		return;
	}
	if (!Target)
	{ 
		return;
	}
	if (Sender->NoCollision)
	{ 
		return;
	}
	if (Target->NoCollision)
	{
		return;
	}
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
			CallbackOwner->OnBeginOverlap(Sender->CollisionTag, Target->CollisionTag, Target->Owner);
		}
	}
	else if (!Ignore && 
		Sender->IsOverlapped)
	{
		Sender->IsOverlapped = false;
		CallbackOwner->OnEndOverlap(Sender->CollisionTag, Target->CollisionTag);
	}
}

void AssignCollisionRectangleToRender(
	ID2D1HwndRenderTarget* Renderer, CollisionComponent* CollisionRectToRender)
{
	if (!CollisionRectToRender->RenderCollisionRect)
	{
		return;
	}

	// "1" is alpha value
	const D2D1_COLOR_F Color =
		{ CollisionRectToRender->CollisionRectColor.R,
		CollisionRectToRender->CollisionRectColor.G,
		CollisionRectToRender->CollisionRectColor.B, 1 };

	ID2D1SolidColorBrush* Brush;
	Renderer->CreateSolidColorBrush(Color, &Brush);

	Brush->SetOpacity(CollisionRectToRender->Opacity);

	D2D1_RECT_F Rect = D2D1::RectF(
		CollisionRectToRender->ComponentLocation.X,
		CollisionRectToRender->ComponentLocation.Y,
		CollisionRectToRender->ComponentLocation.X +
		CollisionRectToRender->CollisionRect.X,
		CollisionRectToRender->ComponentLocation.Y +
		CollisionRectToRender->CollisionRect.Y);

	if (CollisionRectToRender->DrawFilledRectangle)
	{
		Renderer->FillRectangle(Rect, Brush);
	}
	else
	{
		Renderer->DrawRectangle(Rect, Brush);
	}

	Brush->Release();
}

void RenderCollisionRectangles(ID2D1HwndRenderTarget* Renderer,
	std::vector<CollisionComponent*> CollisionRectsToRender)
{
	for (int i = 0; i < CollisionRectsToRender.size(); i++)
	{
		AssignCollisionRectangleToRender(Renderer, CollisionRectsToRender[i]);
	}
}

void RenderCollisionRectangle(ID2D1HwndRenderTarget* Renderer,
	CollisionComponent* CollisionRectToRender)
{
	AssignCollisionRectangleToRender(Renderer, CollisionRectToRender);
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

bool SetDebugMode()
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

bool SetEditorMode()
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

void SaveLevelFile(VoodooEngine* Engine)
{
	SaveGameObjectsToFile(Engine->FileName, Engine);
}

void OpenLevelFile(VoodooEngine* Engine)
{
	OPENFILENAMEA OpenFileName = { sizeof(OPENFILENAMEA) };
	OpenFileName.lStructSize = sizeof(OPENFILENAMEA);
	OpenFileName.hwndOwner = Engine->Window.HWind;
	OpenFileName.lpstrFile = Engine->FileName;
	OpenFileName.lpstrTitle = "Level To Open";
	OpenFileName.lpstrFilter = "Lev File\0*.LEV\0";
	OpenFileName.nMaxFile = sizeof(Engine->FileName);

	// If "cancel" button was pressed, then don't load assets
	if (!GetOpenFileNameA(&OpenFileName))
	{
		return;
	}
	else
	{
		// Pass empty vector since it is only used for storing gameobjects to levels
		std::vector<GameObject*> EmptyVector;
		// Called once "open" button has been clicked
		LoadGameObjectsFromFile(Engine->FileName, Engine, EmptyVector);
	}
}

void SaveGameObjectsToFile(char* FileName, VoodooEngine* Engine)
{
	std::ofstream File(FileName);
	if (File.is_open())
	{
		for (int i = 0; i < Engine->StoredGameObjects.size(); i++)
		{
			File << Engine->StoredGameObjects[i]->GameObjectID 
				<< " " << Engine->StoredGameObjects[i]->Location.X 
				<< " " << Engine->StoredGameObjects[i]->Location.Y << '\n';
		}
	}

	File.close();
}

void LoadGameObjectsFromFile(
	char* FileName, VoodooEngine* Engine, 
	std::vector<GameObject*>& LevelToAddGameObjects, bool DeleteExistingObjectsOnLoad)
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
			
			Engine->AssetLoadFunctionPointer(GameObjectID, SpawnLocation, LevelToAddGameObjects);
		}
	}

	File.close();
}

void InitWindow(
	VoodooEngine* Engine,
	LPCWSTR WindowTitle, 
	WNDPROC WindowProcedure,
	int WindowResolutionWidth, 
	int WindowResolutionHeight, 
	bool WindowFullScreen)
{
	// Setup the window
	Engine->Window.WindowTitle = WindowTitle;
	Engine->Window.ScreenResolutionWidth = WindowResolutionWidth;
	Engine->Window.ScreenResolutionHeight = WindowResolutionHeight;
	Engine->Window.Fullscreen = WindowFullScreen;
	CreateAppWindow(Engine->Window, WindowProcedure);

	// Setup the renderer
	Engine->Renderer = SetupRenderer(Engine->Renderer, Engine->Window.HWind);
}

void InitEngine(VoodooEngine* Engine)
{
	// Assign based on configuration file if debug mode is true/false
	Engine->DebugMode = SetDebugMode();
	// Assign based on configuration file if editor mode is true/false
	Engine->EditorMode = SetEditorMode();

	// Set the app icon that is visible in task bar and window title bar
	SetCustomAppIcon(Engine);

	// Create engine mouse cursor
	CreateMouse(Engine, { 6, 6 });

	// Setup default brushes used by various objects that needs a brush 
	// (so we don't create new brushes for every object)
	SetupDefaultBrushes(Engine);

	// Create the text format for the engine UI texts
	CreateUITextFormat(Engine);
	
	// Set up the frequency (only need to do once)
	QueryPerformanceFrequency(&Engine->TicksPerSecond);
	// Set the start ticks for calculating frame rate
	QueryPerformanceCounter(&Engine->StartTicks);

	Engine->EngineRunning = true;

	// Create level editor if in editor mode 
	// (no need to store the pointer, 
	// since it will be kept in memory throughout the lifetime of the application)
	if (Engine->EditorMode)
	{
		LevelEditor* LevelEditorInstance = new LevelEditor(Engine);
	}
}

void RenderLevelEditor(VoodooEngine* Engine)
{
	if (!Engine->EditorMode)
	{ 
		return;
	}

	RenderBitmaps(Engine->Renderer, Engine->StoredEditorBitmapComponents);
	RenderBitmaps(Engine->Renderer, Engine->StoredButtonBitmapComponents);
	RenderBitmaps(Engine->Renderer, Engine->StoredButtonTexts);
}

void Render(VoodooEngine* Engine)
{
	// Render the game background first (painter's algorithm)
	RenderBitmap(Engine->Renderer, Engine->CurrentGameBackground);

	// Render all bitmaps (from gameobjects) stored in engine
	RenderBitmaps(Engine->Renderer, Engine->StoredBitmapComponents, RENDERLAYER_MAXNUM);
	// Render all collision rects
	RenderCollisionRectangles(
		Engine->Renderer, Engine->StoredCollisionComponents);

	// Call render interface to all inherited objects
	for (int i = 0; i < Engine->InterfaceObjects_Render.size(); i++)
	{
		Engine->InterfaceObjects_Render[i]->OnRenderBroadcast(Engine->Renderer);
	}
	// Render editor related stuff
	if (Engine->EditorMode)
	{
		RenderLevelEditor(Engine);
		RenderUITextsRenderLayer(Engine);
	}
	// Render debug related stuff
	if (Engine->DebugMode)
	{
		RenderCollisionRectangles(
			Engine->Renderer, Engine->StoredEditorCollisionComponents);
		RenderBitmaps(
			Engine->Renderer, Engine->StoredScreenPrintTexts);
	}
	// This replaces the default windows system mouse cursor (it's hidden)
	RenderCustomMouseCursor(Engine->Renderer, Engine);
}

void RunEngine(VoodooEngine* Engine)
{
	if (!Engine)
	{
		return;
	}

	Update(Engine);
	
	Engine->Renderer->BeginDraw();
	Engine->Renderer->Clear(Engine->ClearScreenColor);
	Render(Engine);
	Engine->Renderer->EndDraw();
}

float Interpolate(float Current, float Target, float DeltaTime, float Speed)
{
	return Current + Speed * (Target - Current) * DeltaTime;
}

void SetGameObjectLocation(GameObject* GameObjectToSet, SVector NewLocation)
{
	if (!GameObjectToSet)
	{
		return;
	}

	GameObjectToSet->Location = NewLocation;
	GameObjectToSet->GameObjectBitmap.ComponentLocation = NewLocation;
	GameObjectToSet->AssetCollision.ComponentLocation = NewLocation;
}

void SetCharacterLocation(Character* CharacterToSet, SVector NewLocation)
{
	if (!CharacterToSet)
	{
		return;
	}

	// Check to see if gravity was set to be activated for this character
	bool WasGravityEnabled = CharacterToSet->MoveComp.GravityEnabled;

	// Temp disable gravity to ensure player gets teleported to new location
	CharacterToSet->MoveComp.GravityEnabled = false;
	CharacterToSet->MoveComp.Velocity = 0;

	// Teleport player to new location
	CharacterToSet->Location = NewLocation;
	CharacterToSet->GameObjectBitmap.ComponentLocation = NewLocation;
	CharacterToSet->AssetCollision.ComponentLocation = NewLocation;
	CharacterToSet->MoveComp.QuadCollisionParams.CollisionLeft.ComponentLocation = NewLocation;
	CharacterToSet->MoveComp.QuadCollisionParams.CollisionRight.ComponentLocation = NewLocation;
	CharacterToSet->MoveComp.QuadCollisionParams.CollisionUp.ComponentLocation = NewLocation;
	CharacterToSet->MoveComp.QuadCollisionParams.CollisionDown.ComponentLocation = NewLocation;

	// Only set gravity back if it was set to be activated for this character
	if (WasGravityEnabled)
	{
		CharacterToSet->MoveComp.GravityEnabled = true;
	}
}

SVector AddMovementInput(Character* CharacterToAddMovement, VoodooEngine* Engine)
{	
	// Default new location as the location of the component owner
	SVector NewLocation = CharacterToAddMovement->Location;

	// Add new location on X Axis (left/right)
	if (CharacterToAddMovement->MoveComp.MovementDirection.X != 0)
	{
		if (CharacterToAddMovement->MoveComp.MovementDirection.X < 0 &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitLeft)
		{
			NewLocation.X -= CharacterToAddMovement->MoveComp.MovementSpeed * Engine->DeltaTime;
		}
		if (CharacterToAddMovement->MoveComp.MovementDirection.X > 0 &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitRight)
		{
			NewLocation.X += CharacterToAddMovement->MoveComp.MovementSpeed * Engine->DeltaTime;
		}
	}
	// Add new location on Y Axis (up/down) if allowed
	if (CharacterToAddMovement->MoveComp.MovementDirection.Y != 0 &&
		!CharacterToAddMovement->MoveComp.IsFalling() &&
		!CharacterToAddMovement->MoveComp.IsJumping())
	{
		if (CharacterToAddMovement->MoveComp.MovementDirection.Y < 0 &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp)
		{
			NewLocation.Y -= CharacterToAddMovement->MoveComp.MovementSpeed * Engine->DeltaTime;
		}
		if (CharacterToAddMovement->MoveComp.MovementDirection.Y > 0 &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown)
		{
			NewLocation.Y += CharacterToAddMovement->MoveComp.MovementSpeed * Engine->DeltaTime;
		}
	}

	// Set the default collision detected to false
	CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitLeft = false;
	CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitRight = false;
	CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp = false;
	CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown = false;

	// Check for collision
	for (int i = 0; i < Engine->StoredCollisionComponents.size(); i++)
	{
		// Collision detected left
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionLeft, 
			Engine->StoredCollisionComponents[i]))
		{
			CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitLeft = true;
		}
		// Collision detected right
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionRight,
			Engine->StoredCollisionComponents[i]))
		{
			CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitRight = true;
		}
		// Collision detected up
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionUp, 
			Engine->StoredCollisionComponents[i]))
		{
			CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp = true;
			CharacterToAddMovement->MoveComp.RoofHitCollisionLocation =
				Engine->StoredCollisionComponents[i]->ComponentLocation.Y;
		}
		// Collision detected down
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionDown, 
			Engine->StoredCollisionComponents[i]))
		{
			if (!CharacterToAddMovement->MoveComp.IsRequestingJump())
			{
				CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown = true;
				CharacterToAddMovement->MoveComp.GroundHitCollisionLocation =
					Engine->StoredCollisionComponents[i]->ComponentLocation.Y;
			}
		}
	}
	
	// Update gravity if enabled, used for e.g. sidescroller platformer, 
	// and not used for e.g. top down 4 directional movement
	if (CharacterToAddMovement->MoveComp.GravityEnabled &&
		!CharacterToAddMovement->MoveComp.IsClimbing())
	{
		CharacterToAddMovement->MoveComp.UpdateGravity();
		if (!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown)
		{
			NewLocation.Y += CharacterToAddMovement->MoveComp.Velocity;
		}

		// Snap character to ground if ground "down" collision detected
		if (CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp &&
			!CharacterToAddMovement->MoveComp.IsRequestingJump())
		{
			NewLocation.Y = 
				CharacterToAddMovement->MoveComp.GroundHitCollisionLocation - 
				CharacterToAddMovement->GameObjectDimensions.Y;
		}
		// Prevent character from clipping into roof if "up" collision is detected
		else if (CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown)
		{
			NewLocation.Y = 
				CharacterToAddMovement->MoveComp.RoofHitCollisionLocation + 
				CharacterToAddMovement->GameObjectDimensions.Y;
		}	
	}

	// Update the new location of the quad collision rects 
	// that check for character collision with the environment
	CharacterToAddMovement->MoveComp.UpdateQuadCollisionLocation(NewLocation);

	// Set character bitmap and asset collision location the same as the new location
	CharacterToAddMovement->Location.X = NewLocation.X;
	CharacterToAddMovement->GameObjectBitmap.ComponentLocation.X = NewLocation.X;
	CharacterToAddMovement->Location.Y = NewLocation.Y;
	CharacterToAddMovement->GameObjectBitmap.ComponentLocation.Y = NewLocation.Y;
	CharacterToAddMovement->AssetCollision.ComponentLocation.X = NewLocation.X;
	CharacterToAddMovement->AssetCollision.ComponentLocation.Y = NewLocation.Y;

	return NewLocation;
}

SVector AddMovementAI(AIComponent& AIComp)
{
	SVector NewLocation;


	return NewLocation;
}
