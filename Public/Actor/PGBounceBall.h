#pragma once

#include "Actor/PGCharacter.h"
#include "Core/Components/TDelegate.h"

class APGViewport;

class APGBounceBall : public APGCharacter
{
public:
	virtual ~APGBounceBall() = default;

	void Tick(const float Delta) override;

	void InitializeBallProperties(APGViewport* NewParent, const FGameSettings& Settings, const sf::Vector2u& RWSize);

	void AddTrackedObstacle(APGCharacter* Obstacle) { CachedObstacles.push_back(Obstacle); };

	void Reset();

	TDelegate<> OnPlayerScored;
	TDelegate<> OnEnemyScored;
	TDelegate<APGCharacter*> OnBounce;

protected:
	friend TStrongPtr<APGBounceBall>;
	APGBounceBall() = default;

	virtual void HandleWallCollision(sf::Vector2f& Position);
	virtual void HandleCharacterCollision(sf::Vector2f& Position);
	virtual void HandleBounce(const sf::FloatRect& ObstacleRect, sf::Vector2f& Position, float Radius);

	std::vector<APGCharacter*> CachedObstacles;

	sf::Vector2f BallResetPosition = sf::Vector2f();
	sf::Vector2f BallInitialDirectionYFan = sf::Vector2f();

	void OnGameDifficultyChanged(const FGameDifficulty& GameDifficulty) override;

	float Accention = 1.f;

private:
	bool CircleIntersectsRect(sf::Vector2f& Pos, float Radius, const sf::FloatRect& Rect);
};