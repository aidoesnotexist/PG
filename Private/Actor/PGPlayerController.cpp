#include "Actor/PGPlayerController.h"

void APGPlayerController::Tick(const float Delta)
{
	for (const sf::Event& Input : InputEvents)
	{
		if (Input.type == sf::Event::KeyPressed)
		{
			if (Input.key.code == sf::Keyboard::Escape)
			{
				OnEscape.Broadcast();
			}
		}

		if (Input.type == sf::Event::MouseButtonReleased)
		{
			OnMouseEvent.Broadcast(EMouseInput::I::MouseButtonReleased, sf::Vector2f(static_cast<float>(Input.mouseButton.x), static_cast<float>(Input.mouseButton.y)));
		}
	}

	float Forward = 0.f;
	float Right = 0.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) Forward -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) Forward += 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) Right -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) Right += 1.f;

	if (Forward != 0.f)
	{
		OnMoveEvent.Broadcast(EUserInput::I::MoveForward, Forward);
	}
	if (Right != 0.f)
	{
		OnMoveEvent.Broadcast(EUserInput::I::MoveRight, Right);
	}
}

