#pragma once

#include "Core/Object.h"
#include <cassert>

class UPGInstance : public UPGObject
{
public:
	virtual void Initialize() {}
	virtual void Deinitialize() {}

	virtual ~UPGInstance() = default;

protected:
	friend TStrongPtr<UPGInstance>;
	UPGInstance() = default;
};