#pragma once

#include "Instance/PGInstance.h"

class UPGGameInstance;

//Abstract base class for GameInstance subsystems
class UPGGameInstanceSubsystem abstract : public UPGInstance
{
public:
	virtual void PostInit(UPGGameInstance* GameInstance) = 0;
};