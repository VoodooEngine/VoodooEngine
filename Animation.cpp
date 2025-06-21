#include "Animation.h"

void SetAnimationState(SAnimationParameters& AnimationParams,
	SVector& BitmapSource, SVector& BitmapOffsetLeft, SVector& BitmapOffsetRight)
{
	// Goes from top to bottom in a spritesheet depending on desired animation state
	// e.g. idle is at top row of spritesheet since that "AnimationState" number is 0, 
	// walk is a row below since "AnimationState" number is 1 etc. 
	BitmapSource.Y = AnimationParams.FrameHeight * AnimationParams.AnimationState;
	BitmapOffsetLeft.Y = BitmapSource.Y - AnimationParams.FrameHeight;
	BitmapOffsetRight.Y = AnimationParams.FrameHeight;
}

void UpdateAnimation(SAnimationParameters& AnimationParams,
	SVector& BitmapSource, SVector& BitmapOffsetLeft, SVector& BitmapOffsetRight, float DeltaTime)
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

void InitAnimationFirstFrame(SAnimationParameters& AnimationParams,
	SVector& BitmapSource, SVector& BitmapOffsetLeft, SVector& BitmapOffsetRight)
{
	UpdateAnimation(AnimationParams, BitmapSource, BitmapOffsetLeft, BitmapOffsetRight, 1);
}
