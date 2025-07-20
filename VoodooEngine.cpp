#include "VoodooEngine.h"

// Create and register app window
static void CreateAppWindow(SWindowParameters& WindowParams, WNDPROC InputCallbackFunction)
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

void CreateText(VoodooEngine* Engine, Button* ButtonReference, SButtonParameters ButtonParams)
{
	SEditorAssetPathList FontAssetPath;

	SVector LetterLocation = ButtonParams.ButtonLocation;
	LetterLocation.Y = ButtonParams.ButtonLocation.Y + ButtonParams.ButtonTextOffset.Y;
	float LetterOffsetX = LetterLocation.X + ButtonParams.ButtonTextOffset.X;
	for (int i = 0; i < ButtonParams.ButtonTextString.length(); ++i)
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

void ScreenPrint(VoodooEngine* Engine, std::string DebugText)
{
	if (!Engine)
	{
		return;
	}

	SEditorAssetPathList FontAssetPath;

	float OriginPositionY = 100;
	SVector LetterLocation = { 0, OriginPositionY };
	float LetterOffsetX = LetterLocation.X;
	Engine->ScreenPrintTextColumnsPrinted += 1;
	float OffsetAmount = 30;
	float LetterOffsetY = LetterLocation.Y += (OffsetAmount * Engine->ScreenPrintTextColumnsPrinted);
	for (int i = 0; i < DebugText.length(); ++i)
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
	SVector ButtonLocation, 
	const wchar_t* AssetPath,
	float TextureAtlasHeight,
	float TextureAtlasOffsetYMultiplier)
{
	// Create button class and setup button parameters
	ButtonToCreate = new Button();
	ButtonToCreate->ButtonParams.ButtonCollisionTag = ButtonID;
	ButtonToCreate->ButtonParams.ButtonType = ButtonType;
	ButtonToCreate->ButtonParams.ButtonTextString = ButtonText;
	ButtonToCreate->ButtonParams.ButtonLocation = ButtonLocation;
	ButtonToCreate->ButtonParams.AssetPathButtonBitmap = AssetPath;

	ButtonToCreate->ButtonBitmap.Bitmap =
		SetupBitmap(ButtonToCreate->ButtonBitmap.Bitmap,
		ButtonToCreate->ButtonParams.AssetPathButtonBitmap,
		Engine->Renderer);

	if (ButtonType == AssetButtonThumbnail)
	{
		SetupBitmapComponent(
			&ButtonToCreate->ButtonBitmap, ButtonToCreate->ButtonBitmap.Bitmap,
			{ 90, TextureAtlasHeight }, TextureAtlasOffsetYMultiplier, false);
	}
	else
	{
		SetupBitmapComponent(
			&ButtonToCreate->ButtonBitmap, ButtonToCreate->ButtonBitmap.Bitmap);
	}

	ButtonToCreate->ButtonBitmap.ComponentLocation = ButtonToCreate->ButtonParams.ButtonLocation;	

	// Setup bitmap source based on button type
	SVector BitmapVector2D = { 0, 0 };
	switch (ButtonType)
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
		BitmapVector2D = Engine->AssetButtonThumbnailDimensions;
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
			++ButtonToDeleteIndex)
		{
			BitmapComponent* CurrentButtonText = 
				ButtonToDelete->ButtonText[ButtonToDeleteIndex];

			for (int EngineStoredButtonsIndex = 0;
				EngineStoredButtonsIndex < Engine->StoredButtonTexts.size(); 
				++EngineStoredButtonsIndex)
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
	for (int i = 0; i < ButtonTextToUpdate->ButtonText.size(); ++i)
	{
		switch (ButtonState)
		{
		case Default:
			ButtonTextToUpdate->ButtonText[i]->BitmapParams.BitmapSetToNotRender = false;
			break;
		case Disabled:
			ButtonTextToUpdate->ButtonText[i]->BitmapParams.BitmapSetToNotRender = false;
			break;
		case Hidden:
			ButtonTextToUpdate->ButtonText[i]->BitmapParams.BitmapSetToNotRender = true;
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
		ButtonToUpdate->ButtonBitmap.BitmapParams.BitmapSetToNotRender = false;
		ButtonToUpdate->AdditionalBackgroundBitmap.BitmapParams.BitmapSetToNotRender = false;
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
		ButtonToUpdate->ButtonBitmap.BitmapParams.BitmapSetToNotRender = false;
		ButtonToUpdate->AdditionalBackgroundBitmap.BitmapParams.BitmapSetToNotRender = false;
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
		ButtonToUpdate->ButtonBitmap.BitmapParams.BitmapSetToNotRender = true;
		ButtonToUpdate->AdditionalBackgroundBitmap.BitmapParams.BitmapSetToNotRender = true;
		ButtonToUpdate->ButtonCollider.NoCollision = true;
		SetButtonText(ButtonToUpdate, NewButtonState);
		break;
	}
}

void SetMouseColliderSize(VoodooEngine* Engine, SVector ColliderSize)
{
	Engine->Mouse.MouseCollider.CollisionRect = ColliderSize;
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

void Update(VoodooEngine* Engine)
{
	UpdateFrameRate(Engine);
	UpdateAppWindow();
	UpdateCustomMouseCursorLocation(Engine);

	if (Engine->EditorMode)
	{
		for (int i = 0; i < Engine->StoredEditorUpdateComponents.size(); ++i)
		{
			Engine->StoredEditorUpdateComponents[i]->Update(Engine->DeltaTime);
		}
	}

	if (Engine->GameRunning)
	{
		for (int i = 0; i < Engine->StoredUpdateComponents.size(); ++i)
		{
			if (!Engine->StoredUpdateComponents[i]->Paused)
			{
				Engine->StoredUpdateComponents[i]->Update(Engine->DeltaTime);
			}
		}

		// Only used for timers
		for (int i = 0; i < Engine->StoredTimerUpdateComponents.size(); ++i)
		{
			if (!Engine->StoredTimerUpdateComponents[i]->Paused)
			{
				Engine->StoredTimerUpdateComponents[i]->Update(Engine->DeltaTime);
			}
		}
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

void PauseGame(VoodooEngine* Engine, bool SetGamePaused)
{
	for (int i = 0; i < Engine->StoredUpdateComponents.size(); ++i)
	{
		Engine->StoredUpdateComponents[i]->Paused = SetGamePaused;
	}
}

bool SetDebugMode()
{
	bool NewDebugMode = false;
	std::fstream File("Config.txt");
	std::string String;
	if (File.is_open())
	{
		while (getline(File, String))
		{
			if (String == "DebugMode = TRUE")
			{
				NewDebugMode = true;
			}
			else if (String == "DebugMode = FALSE")
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
	std::fstream File("Config.txt");
	std::string String;
	if (File.is_open())
	{
		while (getline(File, String))
		{
			if (String == "EditorMode = TRUE")
			{
				NewEditorMode = true;
			}
			else if (String == "EditorMode = FALSE")
			{
				NewEditorMode = false;
			}
		}
	}
	File.close();

	return NewEditorMode;
}

void StoreTextureAtlasesFromFile(VoodooEngine* Engine)
{
	// Default to none 
	// (negative values is not accounted for)
	int TextureAtlasID = -1;

	const wchar_t* FileName = L"GameContent/Data/TextureAtlasID.txt";
	std::fstream File(FileName);
	if (File.is_open())
	{
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

			// Assign texture atlas ID
			TextureAtlasID = std::stoi(HorizontalLineNum[0]);

			// Assign asset path
			std::wstring WideStringAssetPath = std::wstring(
				HorizontalLineNum[1].begin(), HorizontalLineNum[1].end());
			const wchar_t* AssetPath = WideStringAssetPath.c_str();

			// Assign texture atlas
			BitmapComponent TextureAtlas;
			TextureAtlas.Bitmap = SetupBitmap(
				TextureAtlas.Bitmap, AssetPath, Engine->Renderer);
			SetupBitmapComponent(&TextureAtlas, TextureAtlas.Bitmap);

			// Store texture atlas
			Engine->StoredAssetTextureAtlases[TextureAtlasID] = { TextureAtlas, AssetPath, WideStringAssetPath };
		}
	}

	File.close();
}

void StoreGameObjectIDsFromFile(VoodooEngine* Engine)
{
	// Default to none 
	// (negative values is not accounted for)
	int GameObjectID = -1;
	
	const wchar_t* FileName = L"GameContent/Data/GameObjectID.txt";
	std::fstream File(FileName);
	if (File.is_open())
	{
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

			// Get gameobject ID 
			GameObjectID = std::stoi(HorizontalLineNum[0]);

			// Get the texture atlas
			auto Iterator = Engine->StoredAssetTextureAtlases.find(std::stoi(HorizontalLineNum[1]));

			// Get desired collision
			bool CreateCollision = std::stoi(HorizontalLineNum[6]);

			// Store game object ID
			Engine->StoredGameObjectIDs[GameObjectID] =
				{ Iterator->second.TextureAtlas.Bitmap,
				std::stof(HorizontalLineNum[2]),
				std::stof(HorizontalLineNum[3]),
				std::stoi(HorizontalLineNum[4]),
				std::stoi(HorizontalLineNum[5]),
				CreateCollision,
				Iterator->second.TextureAtlasPathString,
				std::stof(HorizontalLineNum[7]),
				std::stof(HorizontalLineNum[8]) };
		}
	}

	File.close();
}

void ActivateLevel(
	VoodooEngine* Engine,
	std::vector<GameObject*>& Level,
	int PlayerID,
	int PlayerStartLeftID,
	int PlayerStartRightID,
	int PlayerStartUpID,
	int PlayerStartDownID,
	BitmapComponent* LevelBackground)
{
	if (LevelBackground)
	{
		Engine->CurrentLevelBackground = LevelBackground;
	}

	// First disable and hide all game objects (except player)
	for (int i = 0; i < Engine->StoredGameObjects.size(); ++i)
	{
		if (Engine->StoredGameObjects[i]->GameObjectID == PlayerID)
		{
			continue;
		}

		Engine->StoredGameObjects[i]->UpdateGameObjectState(false);

		// If in debug mode stop rendering the debug asset collision rect
		if (Engine->DebugMode)
		{
			Engine->StoredGameObjects[i]->DefaultGameObjectCollision.RenderCollisionRect = false;
		}
	}
	
	// Make the player start objects disabled as default when activating a new level,
	// in case a level does not contain player start objects
	Engine->PlayerStartObjectLeft = nullptr;
	Engine->PlayerStartObjectRight = nullptr;
	Engine->PlayerStartObjectUp = nullptr;
	Engine->PlayerStartObjectDown = nullptr;

	// Now only enable and show game objects in current active level
	for (int i = 0; i < Level.size(); ++i)
	{
		Level[i]->UpdateGameObjectState(true);

		// If in debug mode, render asset collision that is part of the current active level
		if (Engine->DebugMode)
		{
			Level[i]->DefaultGameObjectCollision.RenderCollisionRect = true;
		}

		if (Level[i]->GameObjectID == PlayerStartLeftID)
		{
			Engine->PlayerStartObjectLeft = Level[i];
			Engine->PlayerStartObjectLeft->UpdateGameObjectState(false);
		}
		if (Level[i]->GameObjectID == PlayerStartRightID)
		{
			Engine->PlayerStartObjectRight = Level[i];
			Engine->PlayerStartObjectRight->UpdateGameObjectState(false);
		}
		if (Level[i]->GameObjectID == PlayerStartUpID)
		{
			Engine->PlayerStartObjectUp = Level[i];
			Engine->PlayerStartObjectUp->UpdateGameObjectState(false);
		}
		if (Level[i]->GameObjectID == PlayerStartDownID)
		{
			Engine->PlayerStartObjectDown = Level[i];
			Engine->PlayerStartObjectDown->UpdateGameObjectState(false);
		}
	}

	for (int i = 0; i < Engine->InterfaceObjects_LevelActivated.size(); ++i)
	{
		Engine->InterfaceObjects_LevelActivated[i]->InterfaceEvent_LevelActivated();
	}
}

void InitWindowAndRenderer(
	VoodooEngine* Engine,
	LPCWSTR WindowTitle, 
	WNDPROC WindowsProcedure,
	int WindowResolutionWidth, 
	int WindowResolutionHeight, 
	bool WindowFullScreen)
{
	// Setup the window
	Engine->Window.WindowTitle = WindowTitle;
	Engine->Window.ScreenResolutionWidth = WindowResolutionWidth;
	Engine->Window.ScreenResolutionHeight = WindowResolutionHeight;
	Engine->Window.Fullscreen = WindowFullScreen;
	CreateAppWindow(Engine->Window, WindowsProcedure);

	// Setup the renderer
	Engine->Renderer = SetupRenderer(Engine->Renderer, Engine->Window.HWind);
}

// Store the player start game objects in the asset content browser in the level editor (left, right, up, down). 
// The objects are placed in levels and used to "teleport" player 
// to the location of the player start objects on level load, 
// based on which direction the player was coming from.
// E.g. if player goes to the right limit of a level and the next level is loaded, 
// then the "left" player start is where the player will be teleported to
void StorePlayerStartGameObjects(VoodooEngine* Engine)
{	
	// Reserve "0" as texture atlas ID for player start
	int PlayerStartTextureAtlasID = 0;

	// Get the asset path of player start
	SEditorAssetPathList AssetPath;

	// Create texture atlas
	BitmapComponent TextureAtlas;
	TextureAtlas.Bitmap = SetupBitmap(
		TextureAtlas.Bitmap, 
		AssetPath.AssetPathPlayerStartBitmap, 
		Engine->Renderer);
	SetupBitmapComponent(&TextureAtlas, TextureAtlas.Bitmap);
	
	// Store texture atlas
	Engine->StoredAssetTextureAtlases[PlayerStartTextureAtlasID] = 
		{ TextureAtlas, AssetPath.AssetPathPlayerStartBitmap };

	// Reserve "0-3" for player start game object ID's
	int PlayerStartID = 0;
	int PlayerStartIconOffsetY = 1;
	for (int i = 0; i < 4; ++i)
	{
		Engine->StoredGameObjectIDs[PlayerStartID] = 
		{ TextureAtlas.Bitmap,
		{ 64, 64 }, 
		PlayerStartIconOffsetY, 
		RENDERLAYER_MAXNUM, 
		false,
		AssetPath.AssetPathPlayerStartBitmap, 
		64, 
		(float)PlayerStartIconOffsetY };
		PlayerStartIconOffsetY++;
		PlayerStartID++;
	}
}

void AssignLevelEditorRenderLayerNames(VoodooEngine* Engine, SRenderLayerNames RenderLayerNames)
{
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_0] =
		{ RenderLayerNames.RenderlayerName_0 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_1] =
		{ RenderLayerNames.RenderlayerName_1 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_2] =
		{ RenderLayerNames.RenderlayerName_2 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_3] =
		{ RenderLayerNames.RenderlayerName_3 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_4] =
		{ RenderLayerNames.RenderlayerName_4 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_5] =
		{ RenderLayerNames.RenderlayerName_5 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_6] =
		{ RenderLayerNames.RenderlayerName_6 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_7] =
		{ RenderLayerNames.RenderlayerName_7 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_8] =
		{ RenderLayerNames.RenderlayerName_8 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_9] =
		{ RenderLayerNames.RenderlayerName_9 };
	Engine->StoredLevelEditorRenderLayers[RENDERLAYER_10] =
		{ RenderLayerNames.RenderlayerName_10 };
};

void InitEngine(VoodooEngine* Engine, SRenderLayerNames RenderLayerNames)
{
	// Assign the render layer names and ID's
	AssignLevelEditorRenderLayerNames(Engine, RenderLayerNames);

	// Setup player start game objects
	StorePlayerStartGameObjects(Engine);

	// Setup texture atlases and game object ID's from files
	StoreTextureAtlasesFromFile(Engine);
	StoreGameObjectIDsFromFile(Engine);

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

void SetGameObjectLocation(GameObject* GameObjectToSet, SVector NewLocation)
{
	if (!GameObjectToSet)
	{
		return;
	}

	GameObjectToSet->Location = NewLocation;
	GameObjectToSet->GameObjectBitmap.ComponentLocation = NewLocation;
	GameObjectToSet->DefaultGameObjectCollision.ComponentLocation = NewLocation;
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
	CharacterToSet->DefaultGameObjectCollision.ComponentLocation = NewLocation;
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

SVector AddMovementInput(VoodooEngine* Engine, Character* CharacterToAddMovement)
{	
	// Default new location as the location of the component owner
	SVector NewLocation = CharacterToAddMovement->Location;

	// Add new location on X Axis (left/right) if allowed
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
	for (int i = 0; i < Engine->StoredCollisionComponents.size(); ++i)
	{
		// Don't block character if found collision type is overlap
		if (Engine->StoredCollisionComponents[i]->CollisionType == ECollisionType::Collision_Overlap)
		{
			continue;
		}

		// Collision detected left
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionLeft, 
			Engine->StoredCollisionComponents[i]) &&
			
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionRight &&
			Engine->StoredCollisionComponents[i] != 
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionUp &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionDown)
		{
			CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitLeft = true;
			CharacterToAddMovement->MoveComp.WallLeftHitCollisionLocation =
				CharacterToAddMovement->Location.X;
		}
		// Collision detected right
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionRight,
			Engine->StoredCollisionComponents[i]) &&

			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionLeft &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionUp &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionDown)
		{
			CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitRight = true;
			CharacterToAddMovement->MoveComp.WallRightHitCollisionLocation =
				CharacterToAddMovement->Location.X;
		}
		// Collision detected up
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionUp, 
			Engine->StoredCollisionComponents[i]) &&

			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionDown &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionLeft &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionRight)
		{
			CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp = true;
			CharacterToAddMovement->MoveComp.RoofHitCollisionLocation =
				CharacterToAddMovement->Location.Y;
		}
		// Collision detected down
		if (IsCollisionDetected(
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionDown, 
			Engine->StoredCollisionComponents[i]) &&

			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionUp &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionLeft &&
			Engine->StoredCollisionComponents[i] !=
			&CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionRight)
		{
			if (!CharacterToAddMovement->MoveComp.IsRequestingJump())
			{
				CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown = true;

				// Cache the collision location of the collided object,
				// this will be used later to determine the "snap" location of the character
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
		if (!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitDown &&
			!CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitUp)
		{
			NewLocation.Y += CharacterToAddMovement->MoveComp.Velocity;
		}

		// Snap character to ground if "down" collision detected
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
			CharacterToAddMovement->MoveComp.Velocity = 0;
			NewLocation.Y = CharacterToAddMovement->MoveComp.RoofHitCollisionLocation + 5;
		}
		
		// Prevent character from going into walls
		if (CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitLeft && 
			CharacterToAddMovement->MoveComp.MovementDirection.X < 0)
		{
			NewLocation.X = CharacterToAddMovement->MoveComp.WallLeftHitCollisionLocation;
		}
		if (CharacterToAddMovement->MoveComp.QuadCollisionParams.CollisionHitRight &&
			CharacterToAddMovement->MoveComp.MovementDirection.X > 0)
		{
			NewLocation.X = CharacterToAddMovement->MoveComp.WallRightHitCollisionLocation;
		}
	}

	// Update the new location of the quad collision rects 
	// that check for character collision with the environment
	CharacterToAddMovement->MoveComp.UpdateQuadCollisionLocation(NewLocation);

	// Set character bitmap and asset collision location the same as the new location
	CharacterToAddMovement->Location.X = NewLocation.X;
	CharacterToAddMovement->Location.Y = NewLocation.Y;
	CharacterToAddMovement->GameObjectBitmap.ComponentLocation.X = NewLocation.X;
	CharacterToAddMovement->GameObjectBitmap.ComponentLocation.Y = NewLocation.Y;
	CharacterToAddMovement->GameObjectFlippedBitmap.ComponentLocation.X = NewLocation.X;
	CharacterToAddMovement->GameObjectFlippedBitmap.ComponentLocation.Y = NewLocation.Y;
	CharacterToAddMovement->DefaultGameObjectCollision.ComponentLocation.X = NewLocation.X;
	CharacterToAddMovement->DefaultGameObjectCollision.ComponentLocation.Y = NewLocation.Y;

	return NewLocation;
}

SVector AddMovementAI(AIComponent& AIComp)
{
	SVector NewLocation;


	return NewLocation;
}
