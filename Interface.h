#pragma once

#include "GameObject.h"
#include <d2d1.h>

// Used for when certain objects wants to render on top of all other game objects regardless of renderlayer
class IRender
{
public:
	virtual void InterfaceEvent_Render(ID2D1HwndRenderTarget* Renderer){};
};

// Generic event interface with the sender as parameter 
class IEventGeneric
{
public:
	virtual void InterfaceEvent_Generic(GameObject* Sender){};
};

// Provides input type taken from Win32 key codes 
// (used the defines provided by the engine to check for any particular input) 
// also provides if that input is pressed/not pressed
class IInput
{
public:
	virtual void InterfaceEvent_Input(int Input, bool Pressed){};
};

// Sends optional interface to inherited game object listeners when game has started/ended
class IGameState
{
public:
	// Optional to setup for each game object
	virtual void InterfaceEvent_GameStart(){};
	virtual void InterfaceEvent_GameEnd(){};
};

// Interface called whenever a level is activated
class ILevelActivated
{
public:
	virtual void InterfaceEvent_LevelActivated(){};
};
