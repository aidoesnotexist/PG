#include "Actor/PGEnemyCharacter.h"
#include "Actor/PGBounceBall.h"

void APGEnemyCharacter::Tick(const float Delta)
{
	if (!TargetBall)
	{
		return;
	}

	sf::Transform& CT = GetActorRenderTarget().Transform;
	sf::Vector2f Position = CT.transformPoint(0.f, 0.f);

	sf::RectangleShape* Shape = static_cast<sf::RectangleShape*>(GetActorRenderTarget().Drawable.get());
	if (!Shape)
	{
		return;
	}

	sf::Vector2f Size = Shape->getSize();

	sf::Vector2f BallPos = TargetBall->GetActorRenderTarget().Transform.transformPoint(0.f, 0.f);

	float MyCenterY = Position.y + Size.y * 0.5f;
	float BallY = BallPos.y;

	float Direction = 0.f;

	const float DeadZone = 5.f;

	float DeltaY = BallY - MyCenterY;

	if (std::abs(DeltaY) > DeadZone)
	{
		Direction = (DeltaY > 0.f) ? 1.f : -1.f;
	}

	Position.y += Direction * MovementVelocity * Delta;

	const sf::FloatRect& Bounds = MovementBounds;

	if (Position.y < Bounds.top)
		Position.y = Bounds.top;

	if (Position.y + Size.y > Bounds.top + Bounds.height)
		Position.y = Bounds.top + Bounds.height - Size.y;

	sf::Transform NewT;
	NewT.translate(Position);
	CT = NewT;
}

void APGEnemyCharacter::OnGameDifficultyChanged(const FGameDifficulty& GameDifficulty)
{
	MovementVelocity = GameDifficulty.EnemySpeed;
}