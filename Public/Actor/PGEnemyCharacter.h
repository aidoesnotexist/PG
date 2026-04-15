#pragma once

#include "Actor/PGCharacter.h"

class APGBounceBall;

class APGEnemyCharacter : public APGCharacter
{
public:
	~APGEnemyCharacter() = default;

	void Tick(const float Delta) override;

	void AddTarget(APGBounceBall* Target) { TargetBall = Target; }

protected:
	friend TStrongPtr<APGEnemyCharacter>;
	APGEnemyCharacter() = default;

	void OnGameDifficultyChanged(const FGameDifficulty& GameDifficulty) override;

	APGBounceBall* TargetBall = nullptr;
};
