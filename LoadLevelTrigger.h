#pragma once

#include "VoodooEngine.h"

enum ELoadLevelTriggerType
{
	LevelTriggerType_None,
	LevelTriggerType_Left,
	LevelTriggerType_Right,
	LevelTriggerType_Up,
	LevelTriggerType_Down
};

class LoadLevelTrigger : public Trigger
{
public:
	LoadLevelTrigger(
		ELoadLevelTriggerType TriggerType = ELoadLevelTriggerType::LevelTriggerType_None,
		int CollisionTag = -1)
	{
		AddTriggerComponentsToEngine();
		LoadLevelTriggerType = TriggerType;
		SetupTrigger(TriggerType, CollisionTag);
	}
	~LoadLevelTrigger()
	{
		RemoveTriggerComponentsFromEngine();
	}

	ELoadLevelTriggerType LoadLevelTriggerType = ELoadLevelTriggerType::LevelTriggerType_None;
	void(*OnLoadLevelTriggerOverlap)(ELoadLevelTriggerType TriggerType) = nullptr;

	void OnBeginOverlap(int SenderCollisionTag, int TargetCollisionTag, Object* Target = nullptr)
	{
		Trigger::OnBeginOverlap(SenderCollisionTag, TargetCollisionTag, Target);

		OnLoadLevelTriggerOverlap(LoadLevelTriggerType);
	}

private:
	void AddTriggerComponentsToEngine()
	{
		VoodooEngine::Engine->StoredCollisionComponents.push_back(&DefaultGameObjectCollision);
		VoodooEngine::Engine->StoredUpdateComponents.push_back(this);
	}
	void RemoveTriggerComponentsFromEngine()
	{
		VoodooEngine::Engine->RemoveComponent(
			(CollisionComponent*)&DefaultGameObjectCollision, &VoodooEngine::Engine->StoredCollisionComponents);
		VoodooEngine::Engine->RemoveComponent(
			(UpdateComponent*)this, &VoodooEngine::Engine->StoredUpdateComponents);
	}
	void SetupTrigger(
		ELoadLevelTriggerType TriggerType = ELoadLevelTriggerType::LevelTriggerType_None,
		int CollisionTag = -1)
	{
		SVector HorzontalTriggerRectSize = { 25, (float)VoodooEngine::Engine->ScreenHeightDefault };
		SVector VerticalTriggerRectSize = { (float)VoodooEngine::Engine->ScreenWidthDefault, 25 };

		switch (TriggerType)
		{
		case LevelTriggerType_None:
			break;
		case LevelTriggerType_Left:
			SetTriggerParameters(CollisionTag, HorzontalTriggerRectSize);
			break;
		case LevelTriggerType_Right:
			SetTriggerParameters(CollisionTag, HorzontalTriggerRectSize);
			SetTriggerLocation({ 1900, 0 });
			break;
		case LevelTriggerType_Up:
			SetTriggerParameters(CollisionTag, VerticalTriggerRectSize);
			break;
		case LevelTriggerType_Down:
			SetTriggerParameters(CollisionTag, VerticalTriggerRectSize);
			SetTriggerLocation({ 0, 1060 });
			break;
		default:
			break;
		}
	}
};
