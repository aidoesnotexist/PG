#pragma once

#include "Actor/PGActor.h"
#include "Core/Components/TDelegate.h"

class APGPlayerController : public APGActor
{
public:
	~APGPlayerController() = default;

	void Tick(const float Delta) override;

	TDelegate<EUserInput::I, const float> OnMoveEvent;
	TDelegate<EMouseInput::I, const sf::Vector2f> OnMouseEvent;
	TDelegate<> OnEscape;

	std::vector<sf::Event> InputEvents;

protected:
	friend TStrongPtr<APGPlayerController>;
	APGPlayerController() = default;
};