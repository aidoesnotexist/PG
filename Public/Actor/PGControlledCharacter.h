#pragma once

#include "Actor/PGCharacter.h"

class APGViewport;

class APGControlledCharacter : public APGCharacter
{
public:
	~APGControlledCharacter() = default;

	void Tick(const float Delta) override;

protected:
	friend TStrongPtr<APGControlledCharacter>;
	APGControlledCharacter() = default;

	APGViewport* Parent;

	virtual void OnMove(EUserInput::I UserInput, const float Value) override;

	float InputForward = 0.f;
	float InputRight = 0.f;
};