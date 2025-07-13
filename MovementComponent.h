#pragma once

#include "VoodooEngine.h"
#include "SVector.h"

struct SQuadCollisionParameters
{
	CollisionComponent CollisionLeft;
	CollisionComponent CollisionRight;
	CollisionComponent CollisionUp;
	CollisionComponent CollisionDown;

	SVector RectSizeCollisionLeft;
	SVector RectSizeCollisionRight;
	SVector RectSizeCollisionUp;
	SVector RectSizeCollisionDown;

	SVector RelativeOffsetCollisionLeft;
	SVector RelativeOffsetCollisionRight;
	SVector RelativeOffsetCollisionUp;
	SVector RelativeOffsetCollisionDown;

	bool CollisionHitLeft = false;
	bool CollisionHitRight = false;
	bool CollisionHitUp = false;
	bool CollisionHitDown = false;
};

class MovementComponent
{
public:
	SVector MovementDirection;
	float MovementSpeed = 100;
	SQuadCollisionParameters QuadCollisionParams;

	// Velocity makes gravity smooth when character is jumping/falling
	float Velocity = 0;
	float JumpHeight = 20;
	float GravityScale = 20;
	bool GravityEnabled = false;

	float WallLeftHitCollisionLocation = 0;
	float WallRightHitCollisionLocation = 0;
	float GroundHitCollisionLocation = 0;
	float RoofHitCollisionLocation = 0;

	void InitMovementComponent(GameObject* ComponentOwner,
		SQuadCollisionParameters DesiredQuadCollisionParams,
		float DesiredMovementSpeed, bool EnableGravity)
	{
		InitCollisionRectangles(ComponentOwner, DesiredQuadCollisionParams);
		MovementSpeed = DesiredMovementSpeed;
		GravityEnabled = EnableGravity;
	}
	void RemoveMovementComponent()
	{
		VoodooEngine::Engine->RemoveComponent(
			&QuadCollisionParams.CollisionLeft, &VoodooEngine::Engine->StoredCollisionComponents);
		VoodooEngine::Engine->RemoveComponent(
			&QuadCollisionParams.CollisionRight, &VoodooEngine::Engine->StoredCollisionComponents);
		VoodooEngine::Engine->RemoveComponent(
			&QuadCollisionParams.CollisionUp, &VoodooEngine::Engine->StoredCollisionComponents);
		VoodooEngine::Engine->RemoveComponent(
			&QuadCollisionParams.CollisionDown, &VoodooEngine::Engine->StoredCollisionComponents);
	}
	void UpdateQuadCollisionLocation(SVector NewLocation)
	{
		UpdateCollisionRectsLocation(NewLocation);
	}
	void UpdateGravity()
	{
		// Disable velocity/falling if gravity is not enabled 
		// (e.g. the character movement is top down 4 directional)
		if (!GravityEnabled)
		{
			Velocity = 0;
			Falling = false;
			return;
		}

		if (QuadCollisionParams.CollisionHitDown)
		{
			Falling = false;
			Jumping = false;

			// Reset velocity when ground is detected and jumping is not requested
			if (!IsRequestingJump())
			{
				Velocity = 0;
			}
		}
		else
		{
			// Continously push velocity (gravity) down
			Velocity += 1;

			Falling = true;

			// As soon as character is in the air, reset jump request
			JumpRequested = false;
		}
	}
	bool IsRequestingJump()
	{
		return JumpRequested;
	}
	// Will only allow jump if gravity is enabled, 
	// and if character is not already jumping/falling
	void Jump()
	{
		if (GravityEnabled &&
			!Jumping &&
			!Falling)
		{
			Jumping = true;
			JumpRequested = true;
			Velocity = -JumpHeight;
		}
	}
	bool IsJumping()
	{
		return Jumping;
	}
	// Force reset jump valus to default
	// (only use this when you want to instant reset jump values e.g. on player death)
	void ForceResetJumpValues()
	{
		Jumping = false;
		JumpRequested = false;
		Falling = false;
		QuadCollisionParams.CollisionHitUp = false;
		QuadCollisionParams.CollisionHitDown = false;
		Velocity = 0;
	}
	// Will only allow climb if not falling or jumping
	void Climb()
	{
		if (ClimbAllowed())
		{
			Climbing = true;
		}
	}
	void StopClimb()
	{
		Climbing = false;
	}
	bool IsClimbing()
	{
		return Climbing;
	}
	bool IsFalling()
	{
		return Falling;
	}

private:
	bool Climbing = false;
	bool Falling = false;
	bool Jumping = false;
	bool JumpRequested = false;

	bool ClimbAllowed()
	{
		bool CanClimb = false;
		if (!Falling &&
			!Jumping)
		{
			CanClimb = true;
		}
		return CanClimb;
	}

	void InitCollisionRectangles(GameObject* ComponentOwner,
		SQuadCollisionParameters DesiredQuadCollisionParams)
	{
		if (VoodooEngine::Engine->DebugMode)
		{
			QuadCollisionParams.CollisionLeft.RenderCollisionRect = true;
			QuadCollisionParams.CollisionRight.RenderCollisionRect = true;
			QuadCollisionParams.CollisionUp.RenderCollisionRect = true;
			QuadCollisionParams.CollisionDown.RenderCollisionRect = true;

			QuadCollisionParams.CollisionLeft.CollisionRectColor = VoodooEngine::Engine->ColorYellow;
			QuadCollisionParams.CollisionRight.CollisionRectColor = VoodooEngine::Engine->ColorYellow;
			QuadCollisionParams.CollisionUp.CollisionRectColor = VoodooEngine::Engine->ColorYellow;
			QuadCollisionParams.CollisionDown.CollisionRectColor = VoodooEngine::Engine->ColorYellow;
		}

		QuadCollisionParams.CollisionLeft.CollisionTag = ComponentOwner->GameObjectID;
		QuadCollisionParams.CollisionRight.CollisionTag = ComponentOwner->GameObjectID;
		QuadCollisionParams.CollisionUp.CollisionTag = ComponentOwner->GameObjectID;
		QuadCollisionParams.CollisionDown.CollisionTag = ComponentOwner->GameObjectID;

		QuadCollisionParams.CollisionLeft.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionLeft;
		QuadCollisionParams.CollisionRight.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionRight;
		QuadCollisionParams.CollisionUp.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionUp;
		QuadCollisionParams.CollisionDown.CollisionRect =
			DesiredQuadCollisionParams.RectSizeCollisionDown;

		QuadCollisionParams.RelativeOffsetCollisionLeft =
			DesiredQuadCollisionParams.RelativeOffsetCollisionLeft;
		QuadCollisionParams.RelativeOffsetCollisionRight =
			DesiredQuadCollisionParams.RelativeOffsetCollisionRight;
		QuadCollisionParams.RelativeOffsetCollisionUp =
			DesiredQuadCollisionParams.RelativeOffsetCollisionUp;
		QuadCollisionParams.RelativeOffsetCollisionDown =
			DesiredQuadCollisionParams.RelativeOffsetCollisionDown;

		VoodooEngine::Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionLeft);
		VoodooEngine::Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionRight);
		VoodooEngine::Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionUp);
		VoodooEngine::Engine->StoredCollisionComponents.push_back(&QuadCollisionParams.CollisionDown);
	}
	void UpdateCollisionRectsLocation(SVector NewLocation)
	{
		QuadCollisionParams.CollisionLeft.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionLeft.X;
		QuadCollisionParams.CollisionLeft.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionLeft.Y;

		QuadCollisionParams.CollisionRight.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionRight.X;
		QuadCollisionParams.CollisionRight.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionRight.Y;

		QuadCollisionParams.CollisionUp.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionUp.X;
		QuadCollisionParams.CollisionUp.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionUp.Y;

		QuadCollisionParams.CollisionDown.ComponentLocation.X =
			NewLocation.X + QuadCollisionParams.RelativeOffsetCollisionDown.X;
		QuadCollisionParams.CollisionDown.ComponentLocation.Y =
			NewLocation.Y + QuadCollisionParams.RelativeOffsetCollisionDown.Y;
	}
};
