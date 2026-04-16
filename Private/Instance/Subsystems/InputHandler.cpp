
#include "Instance/Subsystems/InputHandler.h"
#include "Instance/PGGameInstance.h"
#include "Actor/PGViewport.h"

void UInputHandler::Initialize()
{

}

void UInputHandler::Deinitialize()
{
	
}

void UInputHandler::PostInit(UPGGameInstance* InGameInstance)
{
	if (!InGameInstance)
	{
		return;
	}
	GameInstance = InGameInstance;
}

void UInputHandler::RegisterInputs(const APGViewport* Viewport)
{
	if (!PlayerController || !Viewport || GameInstance->bIsStreaming || GameInstance->bIsPendingExit)
	{
		return;
	}
;
	if (!Viewport->RenderWindow)
	{
		return;
	}

	sf::Event InputEvent;
	PlayerController->InputEvents.clear();

	while (Viewport->RenderWindow->pollEvent(InputEvent))
	{
		PlayerController->InputEvents.push_back(InputEvent);
	}
}

void UInputHandler::ClearInputs()
{
	PlayerController->InputEvents.clear();
}

bool UInputHandler::DestroyPlayerController()
{
	return PlayerController.CheckDelete();
}

