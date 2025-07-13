#pragma once

#include "VoodooEngineDLLExport.h"

// DirectWrite API
#include <dwrite.h>
#pragma comment(lib, "Dwrite.lib")

class VoodooEngine;

enum ETextBrushColorType
{
	WhiteBrush,
	BlackBrush
};

// Struct used to determine the UI text parameters
struct STextParameters
{
	const wchar_t* Text = L"";
	ETextBrushColorType TextRenderType = ETextBrushColorType::WhiteBrush;
	bool HideText = false;
};

// Create the text format to be used by all direct write IU text for the remainder of the program
extern "C" VOODOOENGINE_API void CreateUITextFormat(VoodooEngine* Engine);
