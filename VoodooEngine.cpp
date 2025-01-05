#include "pch.h"
#include "VoodooEngine.h"
#include <commdlg.h>
#include <fstream>
#include <sstream>

void CreateAppWindow(SWindowParams &WindowParams, WNDPROC InputCallbackFunction)
{
	LPCWSTR ClassName = L"Class";
	HINSTANCE HInstance = nullptr;
	WindowParams.WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowParams.WindowClass.lpfnWndProc = InputCallbackFunction;
	WindowParams.WindowClass.cbClsExtra = 0;
	WindowParams.WindowClass.cbWndExtra = 0;
	WindowParams.WindowClass.hInstance = HInstance;
	WindowParams.WindowClass.lpszClassName = ClassName;
	WindowParams.WindowClass.lpszMenuName = nullptr;
	WindowParams.WindowClass.hbrBackground = nullptr;
	WindowParams.WindowClass.hIcon = nullptr;
	WindowParams.WindowClass.hIconSm = nullptr;
	WindowParams.WindowClass.hCursor = nullptr;

	RegisterClassEx(&WindowParams.WindowClass);

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
		StoredCallbacks[i]->InputBroadcast(Input, Pressed);
	}
}

void SendMouseInputBroadcast(VoodooEngine* Engine, int Input, bool Pressed)
{
	for (int i = 0; i < Engine->StoredInputCallbacks.size(); i++)
	{
		// Override input and pressed type
		Engine->StoredInputCallbacks[i]->InputBroadcast(Input, Pressed);
	}
}

void UpdateMouseInput(VoodooEngine* Engine, UINT Message)
{
	if (!Engine)
	{
		return;
	}

	// 1 == Primary mouse input ID
	// 2 == Secondary mouse input ID

	switch (Message)
	{
	// Primary mouse button
	case WM_LBUTTONDOWN:
		Engine->Mouse.PrimaryMousePressed = true;
		SendMouseInputBroadcast(Engine, 1, true);
		break;
	case WM_LBUTTONUP:
		Engine->Mouse.PrimaryMousePressed = false;
		SendMouseInputBroadcast(Engine, 1, false);
		break;
	// Secondary mouse button
	case WM_RBUTTONDOWN:
		Engine->Mouse.SecondaryMousePressed = true;
		SendMouseInputBroadcast(Engine, 2, true);
		break;
	case WM_RBUTTONUP:
		Engine->Mouse.SecondaryMousePressed = false;
		SendMouseInputBroadcast(Engine, 2, false);
		break;
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

void RemoveInputCallback(InputCallback* Component, VoodooEngine* Engine)
{
	Engine->StoredInputCallbacks.erase(std::remove(
		Engine->StoredInputCallbacks.begin(),
		Engine->StoredInputCallbacks.end(), Component));
}

void CloseApp(VoodooEngine* Engine)
{
	Engine->EngineRunning = false;
	Engine->StoredBitmapComponents.clear();
	Engine->StoredCollisionComponents.clear();
	Engine->StoredUpdateComponents.clear();
	Engine->StoredInputCallbacks.clear();
	Engine->StoredInputs.clear();
	delete Engine;
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
	CreatedLetter->Bitmap = SetBitmap(Engine->Renderer, Font);
	CreatedLetter->BitmapParams = SetupBitmapParams(CreatedLetter->Bitmap);
	CreatedLetter->ComponentLocation = LetterLocation;
	AssignLetterShiftByID(LetterString, CreatedLetter, Engine);

	return CreatedLetter;
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
		// (don�t create and leave white space if "_" symbol is found, 
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

void ScreenPrint(std::string DebugText, VoodooEngine* Engine)
{
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
		// (don�t create and leave white space if "_" symbol is found, 
		// but still offset the location for the next letter)
		if (DebugText.substr(i, 1) != "_")
		{
			BitmapComponent* NewLetter = CreateLetter(Engine, DebugText.substr(i, 1),
				LetterLocation, FontAssetPath.DebugFont);

			Engine->StoredScreenPrintTexts.push_back(NewLetter);
		}
	}
}

void ClearScreenPrint(VoodooEngine* Engine)
{
	if (Engine->StoredScreenPrintTexts.empty())
		return;

	while (!Engine->StoredScreenPrintTexts.empty())
	{
		for (int i = 0; i < Engine->StoredScreenPrintTexts.size(); i++)
		{
			BitmapComponent* BitmapPointer = Engine->StoredScreenPrintTexts[i];
			Engine->StoredScreenPrintTexts.erase(std::remove(
				Engine->StoredScreenPrintTexts.begin(),
				Engine->StoredScreenPrintTexts.end(), BitmapPointer));
			delete BitmapPointer;
		}
	}

	Engine->ScreenPrintTextColumnsPrinted = 0;
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
		SetBitmap(Engine->Renderer, ButtonToCreate->ButtonParams.AssetPathButtonBitmap);
	ButtonToCreate->ButtonBitmap.BitmapParams = SetupBitmapParams(ButtonToCreate->ButtonBitmap.Bitmap);
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
			SetBitmap(Engine->Renderer, L"EngineContent/LevelEditor/AssetButtonBase.png");
		ButtonToCreate->AdditionalBackgroundBitmap.BitmapParams = 
			SetupBitmapParams(ButtonToCreate->AdditionalBackgroundBitmap.Bitmap);
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

	Engine->Mouse.MouseBitmap.Bitmap = SetBitmap(
		Engine->Renderer, L"EngineContent/Cursor/CustomMouseCursor.png");

	// Setup custom mouse cursor bitmap if found, otherwise render mouse collider instead
	if (Engine->Mouse.MouseBitmap.Bitmap)
	{
		Engine->Mouse.MouseBitmap.BitmapParams = SetupBitmapParams(Engine->Mouse.MouseBitmap.Bitmap);

		if (Engine->DebugMode)
			Engine->Mouse.MouseCollider.RenderCollisionRect = true;
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

void UpdateMouseLocation(VoodooEngine* Engine, SVector NewLocation)
{
	if (!Engine || !Engine->EngineRunning)
		return;

	Engine->Mouse.Location = NewLocation;

	Engine->Mouse.MouseBitmap.ComponentLocation = NewLocation;

	Engine->Mouse.MouseCollider.ComponentLocation =
		{Engine->Mouse.MouseBitmap.ComponentLocation.X + Engine->Mouse.MouseCollider.CollisionRectOffset.X,
		Engine->Mouse.MouseBitmap.ComponentLocation.Y + Engine->Mouse.MouseCollider.CollisionRectOffset.Y};
}

void UpdateCustomMouseCursor(VoodooEngine* Engine)
{
	if (GetMessagePos())
	{
		POINT MousePosition;
		if (GetCursorPos(&MousePosition))
		{
			SVector NewMousePos = { 0, 0 };
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
	if (Engine->EditorMode)
	{
		for (int i = 0; i < Engine->StoredEditorUpdateComponents.size(); i++)
		{
			Engine->StoredEditorUpdateComponents[i]->Update(DeltaTime);
		}
	}

	if (Engine->GameRunning)
	{
		for (int i = 0; i < Engine->StoredUpdateComponents.size(); i++)
		{
			Engine->StoredUpdateComponents[i]->Update(DeltaTime);
		}
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

ID2D1Bitmap* SetBitmap(ID2D1HwndRenderTarget* RenderTarget, 
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

SBitmapParameters SetupBitmapParams(ID2D1Bitmap* BitmapToSetup)
{
	SBitmapParameters BitmapSetup = {};

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

void InitializeAnimationFirstFrame(SAnimationParameters &AnimationParams, 
	SVector &BitmapSource, SVector &BitmapOffsetLeft, SVector &BitmapOffsetRight)
{
	UpdateAnimation(AnimationParams, BitmapSource, BitmapOffsetLeft, BitmapOffsetRight, 1);
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
	// Render mouse collider as fallback if no custom cursor image file is found
	if (Engine->Mouse.MouseBitmap.Bitmap == nullptr)
	{
		RenderCollisionRectangle(Renderer, &Engine->Mouse.MouseCollider);
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
		Target->NoCollision)
	{
		return false;
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
		return;

	if (!Target)
		return;

	if (Sender->NoCollision)
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

void RenderCollisionRectangle(ID2D1HwndRenderTarget* Renderer, CollisionComponent* CollisionRectangleToRender)
{
	int Alpha = 255;
	const D2D1_COLOR_F Color =
	{CollisionRectangleToRender->CollisionRectColor.R,
	CollisionRectangleToRender->CollisionRectColor.G,
	CollisionRectangleToRender->CollisionRectColor.B, Alpha};

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

void RenderCollisionRectangleMultiple(ID2D1HwndRenderTarget* Renderer,
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

		int Alpha = 255;
		const D2D1_COLOR_F Color = 
			{CollisionRectsToRender[i]->CollisionRectColor.R,
			CollisionRectsToRender[i]->CollisionRectColor.G,
			CollisionRectsToRender[i]->CollisionRectColor.B, Alpha};

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
		// Called once "open" button has been clicked
		LoadGameObjectsFromFile(Engine->FileName, Engine);
	}
}

void SaveGameObjectsToFile(char* FileName, VoodooEngine* Engine)
{
	std::ofstream File(FileName);
	if (File.is_open())
	{
		for (int i = 0; i < Engine->StoredGameObjects.size(); i++)
		{
			File << Engine->StoredGameObjects[i]->GameObjectNumberID 
				<< " " << Engine->StoredGameObjects[i]->Location.X 
				<< " " << Engine->StoredGameObjects[i]->Location.Y << '\n';
		}
	}

	File.close();
}

void LoadGameObjectsFromFile(char* FileName, VoodooEngine* Engine)
{
	// Delete all current game objects
	Engine->DeleteAllGameObjects();

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

			GameObjectID = (std::stoi(HorizontalLineNum[0]));
			SpawnLocation.X = (std::stof(HorizontalLineNum[1]));
			SpawnLocation.Y = (std::stof(HorizontalLineNum[2]));
			
			Engine->AssetLoadFunctionPointer(GameObjectID, SpawnLocation);
		}
	}

	File.close();
}
