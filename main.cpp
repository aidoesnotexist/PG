#pragma once

#include "Core/Components/StrongPtr.h"
#include "Core/Object.h"
#include "Instance/PGGameInstance.h"
#include <cassert>

void main() //No external usage, no arguments, no return type
{
	//Creating main game instance. It will operate above all game components
	TStrongPtr<UPGGameInstance> GameInstance = CreateObject<UPGGameInstance>();
	assert(GameInstance && "Failed to create GameInstance");

	UPGGameInstance* GameInstancePtr = GameInstance.Raw();
	if (GameInstance) //Always valid: TStrongPtr in general cannot be valid while its control block is not
	{
		GameInstancePtr->Initialize();
		GameInstancePtr->BeginPlay();
	}
}