#pragma once

#include "Instance/PGInstance.h"

class UPGGameInstance;

class UPGGameInstanceSubsystem abstract : public UPGInstance
{
public:
	virtual void PostInit(UPGGameInstance* GameInstance) = 0;
};