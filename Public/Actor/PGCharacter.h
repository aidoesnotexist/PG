#pragma once

#include "Actor/PGPawn.h"

class APGCharacter : public APGPawn
{
public:
	virtual ~APGCharacter();

	virtual void SetMovementSpeed(float InMovementSpeed) { MovementVelocity = InMovementSpeed; }
	virtual void SetMoveDirection(const sf::Vector2f& Direction) { MoveDirection = Direction; }

	virtual void SetMovementBounds(const sf::FloatRect& MBounds) { MovementBounds = MBounds; }

	sf::FloatRect GetCharacterBounds();

	void BindWorldDelegates(APGViewport* World);

protected:
	friend TStrongPtr<APGCharacter>;
	APGCharacter() = default;

	float MovementVelocity = 0;
	sf::Vector2f MoveDirection = sf::Vector2f();

	sf::FloatRect MovementBounds;

	virtual void OnMove(EUserInput::I UserInput, const float Delta) override {}
	virtual void OnGameDifficultyChanged(const FGameDifficulty& GameDifficulty);

	void HandleDifficultyChange(const FGameDifficulty& GameDifficulty);
};