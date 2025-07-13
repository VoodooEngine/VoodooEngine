#pragma once

#include "VoodooEngineDLLExport.h"
#include "CollisionComponent.h"
#include "BitmapComponent.h"
#include "SVector.h"
#include "Asset.h"
#include <string> 

class VoodooEngine;

enum EButtonState
{
	Default,
	Disabled,
	Hidden
};

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
// TO DO: need to refactor this to use a texture atlas instead of unique bitmap for every button 
// (costs extra unnecessary memory)
class Button
{
public:
	CollisionComponent ButtonCollider;
	BitmapComponent ButtonBitmap;
	BitmapComponent AdditionalBackgroundBitmap;
	SButtonParameters ButtonParams = {};
	std::vector<BitmapComponent*> ButtonText;
};

// Asset button struct used by level editor
// NOTE: Only numbers as "0" and up can be accounted for as ID numbers for assets,
// since the level editor use negative numbers as ID for internal level editor stuff
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

extern "C" VOODOOENGINE_API Button* CreateButton(
	VoodooEngine* Engine,
	Button* ButtonToCreate,
	int ButtonID,
	EButtonType ButtonType,
	std::string ButtonText,
	SVector ButtonLocation,
	const wchar_t* AssetPath,
	float TextureAtlasHeight = {},
	float TextureAtlasOffsetYMultiplier = {});

extern "C" VOODOOENGINE_API Button* DeleteButton(VoodooEngine* Engine, Button* ButtonToDelete);
extern "C" VOODOOENGINE_API void SetButtonState(
	Button* ButtonToUpdate, EButtonState NewButtonState, bool KeepBitmapOffsetUnchanged = false);
