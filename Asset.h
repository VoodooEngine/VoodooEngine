#pragma once

#include "BitmapComponent.h"
#include <string>

struct SAssetTextureAtlas
{
	BitmapComponent TextureAtlas;
	std::wstring TextureAtlasPath;
};

// Contains all the information for game assets
struct SAssetParameters
{
	ID2D1Bitmap* TextureAtlas = nullptr;
	SVector TextureAtlasWidthHeight = { 0, 0 };
	SVector TextureAtlasOffsetMultiplierWidthHeight = { 1, 1 };
	int RenderLayer = 0;
	bool CreateDefaultAssetCollision = false;

	// Default to asset button empty thumbnail, can be overriden
	const wchar_t* EditorAssetButtonThumbnailFilePath = L"EngineContent/LevelEditor/AssetButtonBase.png";
	
	float AssetButtonThumbnailTextureAtlasHeight = 90;
	float AssetButtonThumbnailTextureAtlasOffsetMultiplierY = 1;
};

// Editor asset path list 
// (Used by the file system when loading assets)
struct SEditorAssetPathList
{
	// Engine
	const wchar_t* GameIcon = L"EngineContent/Ico/GameIcon.ico";
	const wchar_t* EditorIcon = L"EngineContent/Ico/EditorIcon.ico";

	// Font asset paths
	const wchar_t* DefaultFont = L"EngineContent/Font/FontMonogram.png";
	const wchar_t* DebugFont = L"EngineContent/Font/FontMonogramDebug.png";

	// Used for debug only 
	const wchar_t* AnimationSpritesDebug = L"EngineContent/Debug/AnimationTesting.png";

	// Level editor
	//-----------------
	// Level editor UI
	const wchar_t* LevelEditorUITop = L"EngineContent/LevelEditor/LevelEditorUI_Top.png";
	const wchar_t* LevelEditorUIOverlay = L"EngineContent/LevelEditor/LevelEditorUI_Overlay.png";
	const wchar_t* LevelEditorTileIconBase = L"EngineContent/LevelEditor/TileIconBase.png";
	// Buttons
	const wchar_t* LevelEditorButtonW140 =
		L"EngineContent/LevelEditor/ButtonW140.png";
	const wchar_t* LevelEditorButtonActivateDeactivateW140 =
		L"EngineContent/LevelEditor/ActivateDeactivateButtonW140.png";
	const wchar_t* LevelEditorButtonW160 =
		L"EngineContent/LevelEditor/ButtonW160.png";
	const wchar_t* RenderLayerEyeIcon =
		L"EngineContent/LevelEditor/RenderLayerEyeIcon.png";
	// Gizmo
	const wchar_t* Gizmo = L"EngineContent/LevelEditor/Gizmo.png";
	// Player start
	// - The path to the png file of player start bitmap
	// - (used by level editor to determine the player position on game start/level load)
	const wchar_t* AssetPathPlayerStartBitmap =
		L"EngineContent/LevelEditor/PlayerStart_TextureAtlas.png";
	//-----------------
};
