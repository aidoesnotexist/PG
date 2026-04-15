#include "Actor/PGPawn.h"
#include "Actor/PGPlayerController.h"
#include "Instance/PGGameInstance.h"
#include "Instance/Subsystems/InputHandler.h"

APGPawn::~APGPawn()
{
	UPGGameInstance* GameInstance = UPGGameInstance::Get();
	if (!GameInstance)
	{
		return;
	}

	UInputHandler* InputHandler = GameInstance->GetSubsystem<UInputHandler>();
	if (!InputHandler)
	{
		return;
	}

	APGPlayerController* PlayerController = InputHandler->GetAttachedPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->OnMoveEvent.Remove(this);
	PlayerController->OnMouseEvent.Remove(this);
}

void APGPawn::BeginPlay()
{
	UPGGameInstance* GameInstance = UPGGameInstance::Get();
	if (!GameInstance)
	{
		return;
	}

	UInputHandler* InputHandler = GameInstance->GetSubsystem<UInputHandler>();
	if (!InputHandler)
	{
		return;
	}

	APGPlayerController* PlayerController = InputHandler->GetAttachedPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->OnMoveEvent.Bind(this, &APGPawn::HandleMove);
	PlayerController->OnMouseEvent.Bind(this, &APGPawn::HandleMouse);
}

void APGPawn::OnMove(EUserInput::I UserInput, const float Delta)
{

}

void APGPawn::OnMouseEvent(EMouseInput::I MouseInput, const sf::Vector2f CachedGeometry)
{

}

void APGPawn::HandleMove(EUserInput::I UserInput, const float Delta)
{
	OnMove(UserInput, Delta);
}

void APGPawn::HandleMouse(EMouseInput::I MouseInput, const sf::Vector2f CachedGeometry)
{
	OnMouseEvent(MouseInput, CachedGeometry);
}

