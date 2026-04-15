
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

	std::unique_lock<std::shared_mutex> Lock = Viewport->LockWindow();
	sf::RenderWindow* RWindow = Viewport->GetRenderWindow();
	if (!RWindow)
	{
		return;
	}

	sf::Event InputEvent;
	PlayerController->InputEvents.clear();
	//PlayerController->InputEvents = PoolEvents;

	while (RWindow->pollEvent(InputEvent))
	{
		//printf("INPUT HANDLER ACCESSING PC AT: %p\n", (void*)PlayerController);
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

