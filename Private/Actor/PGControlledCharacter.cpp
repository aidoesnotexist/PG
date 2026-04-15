#include "Actor/PGControlledCharacter.h"

void APGControlledCharacter::Tick(const float Delta)
{
	sf::Vector2f Move = sf::Vector2f(InputRight * MovementVelocity * Delta, InputForward * MovementVelocity * Delta);

	if (Move.x == 0.f && Move.y == 0.f)
	{
		InputForward = 0.f;
		InputRight = 0.f;
		return;
	}

	sf::Transform& CT = GetActorRenderTarget().Transform;
	sf::Vector2f Position = CT.transformPoint(0.f, 0.f);

	Position += Move;

	sf::RectangleShape* Shape = static_cast<sf::RectangleShape*>(GetActorRenderTarget().Drawable);
	if (!Shape)
	{
		return;
	}

	sf::Vector2f Size = Shape->getSize();
	const sf::FloatRect& Bounds = MovementBounds;

	if (Position.x < Bounds.left)
		Position.x = Bounds.left;

	if (Position.x + Size.x > Bounds.left + Bounds.width)
		Position.x = Bounds.left + Bounds.width - Size.x;

	if (Position.y < Bounds.top)
		Position.y = Bounds.top;

	if (Position.y + Size.y > Bounds.top + Bounds.height)
		Position.y = Bounds.top + Bounds.height - Size.y;

	sf::Transform NewT;
	NewT.translate(Position);
	CT = NewT;

	InputForward = 0.f;
	InputRight = 0.f;
}

void APGControlledCharacter::OnMove(EUserInput::I UserInput, const float Value)
{
	if (UserInput == EUserInput::I::MoveForward)
	{
		InputForward = Value;
	}
	else if (UserInput == EUserInput::I::MoveRight)
	{
		InputRight = Value;
	}
}