#pragma once

#include "Actor/PGActor.h"
#include "Core/Components/TDelegate.h"

/*
	PlayerController is an invisible actor that is bound to traditional
	viewport that local player can see. It handles all system inputs and
	fires them as delegates(callbacks), which different instances can bind to
	for their own personal usage
*/
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