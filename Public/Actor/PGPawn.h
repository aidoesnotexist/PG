#pragma once

#include "Actor/PGActor.h"
#include "BasicTypes.h"

class APGViewport;

/** Base class for every actor that handles user inputs and can move */
class APGPawn : public APGActor
{
public:
	virtual ~APGPawn();

	virtual void BeginPlay() override;

	void Tick(const float Delta) override {}

	void SetWorld(APGViewport* InWorld) { World = InWorld; }

protected:
	friend TStrongPtr<APGPawn>;
	APGPawn() = default;

	virtual void OnMove(EUserInput::I UserInput, const float Delta);
	virtual void OnMouseEvent(EMouseInput::I MouseInput, const sf::Vector2f CachedGeometry);

	APGViewport* World;

private:
	void HandleMove(EUserInput::I UserInput, const float Delta);
	void HandleMouse(EMouseInput::I MouseInput, const sf::Vector2f CachedGeometry);
};