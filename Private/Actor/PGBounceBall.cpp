#include "Actor/PGBounceBall.h"
#include "Actor/PGViewport.h"

void APGBounceBall::Tick(const float Delta)
{
	sf::Transform& BBT = GetActorRenderTarget().Transform;

	sf::Vector2f Position = BBT.transformPoint(0.f, 0.f);

	Position += MoveDirection * MovementVelocity * Delta;

	HandleWallCollision(Position);
	HandleCharacterCollision(Position);

	sf::Transform NewT;
	NewT.translate(Position);
	BBT = NewT;

	float RWSizeX = World->GetRWindowSize().x;
	if (Position.x < RWSizeX - RWSizeX)
	{
		OnEnemyScored.Broadcast();
		Reset();
	}
	if (Position.x > RWSizeX)
	{
		OnPlayerScored.Broadcast();
		Reset();
	}
}

void APGBounceBall::InitializeBallProperties(APGViewport* InWorld, const FGameSettings& Settings, const sf::Vector2u& RWSize)
{
	if (!InWorld)
	{
		return;
	}

	World = InWorld;

	MovementVelocity = Settings.BallSpeed;

	BallResetPosition = sf::Vector2f(Settings.BallDefaultPositionX * RWSize.x, Settings.BallDefaultPositionY * RWSize.y);
	BallInitialDirectionYFan = sf::Vector2f(Settings.BallInitialDirMinY, Settings.BallInitialDirMaxY);

	Reset();
}

void APGBounceBall::Reset()
{
	sf::Transform BBT;
	BBT.translate(BallResetPosition);

	GetActorRenderTarget().Transform = BBT;

	float DirX = (rand() % 2 == 0) ? -1.f : 1.f;

	float Rand = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	float DirY = BallInitialDirectionYFan.x + Rand * (BallInitialDirectionYFan.y - BallInitialDirectionYFan.x);

	MoveDirection = sf::Vector2f(DirX, DirY);

	float Normal = std::sqrt(MoveDirection.x * MoveDirection.x + MoveDirection.y * MoveDirection.y);
	MoveDirection /= Normal;
}

void APGBounceBall::HandleWallCollision(sf::Vector2f& Position)
{
	sf::CircleShape* Shape = dynamic_cast<sf::CircleShape*>(GetActorRenderTarget().Drawable);

	float Radius = Shape->getRadius();

	float Top = 0.f;
	float Bottom = World->GetRWindowSize().y;

	if (Position.y - Radius <= Top)
	{
		Position.y = Top + Radius;
		MoveDirection.y *= -1.f;
		OnBounce.Broadcast(nullptr);
	}

	if (Position.y + Radius >= Bottom)
	{
		Position.y = Bottom - Radius;
		MoveDirection.y *= -1.f;
		OnBounce.Broadcast(nullptr);
	}
}

void APGBounceBall::HandleCharacterCollision(sf::Vector2f& Position)
{
	sf::CircleShape* Shape = dynamic_cast<sf::CircleShape*>(GetActorRenderTarget().Drawable);

	float Radius = Shape->getRadius();

	sf::FloatRect BallRect(Position.x - Radius, Position.y - Radius, Radius * 2.f, Radius * 2.f);

	for (APGCharacter* Obstacle : CachedObstacles)
	{
		sf::FloatRect ObstacleRect = Obstacle->GetCharacterBounds();

		if (CircleIntersectsRect(Position, Radius, ObstacleRect))
		{
			HandleBounce(ObstacleRect, Position, Radius);
			OnBounce.Broadcast(Obstacle);
			MovementVelocity *= Accention;
			break;
		}
	}
}

void APGBounceBall::HandleBounce(const sf::FloatRect& ObstacleRect, sf::Vector2f& Position, float Radius)
{
	float ObstacleCenterY = ObstacleRect.top + ObstacleRect.height * 0.5f;

	float Distance = Position.y - ObstacleCenterY;

	float Normalized = Distance / (ObstacleRect.height * 0.5f);

	if (Normalized < -1.f) Normalized = -1.f;
	if (Normalized > 1.f)  Normalized = 1.f;

	MoveDirection.x *= -1.f;
	MoveDirection.y = Normalized;

	float Norm = std::sqrt(MoveDirection.x * MoveDirection.x + MoveDirection.y * MoveDirection.y);
	MoveDirection /= Norm;

	if (MoveDirection.x > 0)
	{
		Position.x = ObstacleRect.left + ObstacleRect.width + Radius;
	}
	else
	{
		Position.x = ObstacleRect.left - Radius;
	}
}

void APGBounceBall::OnGameDifficultyChanged(const FGameDifficulty& GameDifficulty)
{
	MovementVelocity = GameDifficulty.BallSpeed;
	Accention = GameDifficulty.BallSpeedAccention;
}

bool APGBounceBall::CircleIntersectsRect(const sf::Vector2f& Pos, float Radius, const sf::FloatRect& Rect)
{
	float ClosestX = std::clamp(Pos.x, Rect.left, Rect.left + Rect.width);
	float ClosestY = std::clamp(Pos.y, Rect.top, Rect.top + Rect.height);

	float Dx = Pos.x - ClosestX;
	float Dy = Pos.y - ClosestY;

	return (Dx * Dx + Dy * Dy) <= (Radius * Radius);
}

