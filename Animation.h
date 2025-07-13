#pragma once

#include "VoodooEngineDLLExport.h"
#include "SVector.h"

// Contains animation info such as frame size/total frames and animation speed/state info 
struct SAnimationParameters
{
	int AnimationState = 1;
	float AnimationSpeed = 1;
	int TotalFrames = 1;
	int FrameWidth = 0;
	int FrameHeight = 0;
	int CurrentFrame = 1;
	float AnimationTimer = 0;
};

// Set animation state e.g. idle, run
extern "C" VOODOOENGINE_API void SetAnimationState(
	SAnimationParameters& AnimationParams,
	SVector& BitmapSource,
	SVector& BitmapOffsetLeft,
	SVector& BitmapOffsetRight);

// Update animation
extern "C" VOODOOENGINE_API void UpdateAnimation(
	SAnimationParameters& AnimationParams,
	SVector& BitmapSource,
	SVector& BitmapOffsetLeft,
	SVector& BitmapOffsetRight,
	float DeltaTime);

// Setup the first frame of animation, 
// to use it, call this function inside "OnGameObjectSetupCompleted" virtual function in any gameobject 
// (used for when an object is created before activation of update component, 
// to make sure animation spritesheet bitmap gets framed with the first animation frame)
extern "C" VOODOOENGINE_API void InitAnimationFirstFrame(
	SAnimationParameters& AnimationParams,
	SVector& BitmapSource, SVector& BitmapOffsetLeft, SVector& BitmapOffsetRight);
