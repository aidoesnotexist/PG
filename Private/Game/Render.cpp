#include "Game/Render.h"
#include "Actor/PGViewport.h"
#include "Actor/PGActor.h"
#include "Core/Components/StrongPtr.h"
#include "Instance/Subsystems/InputHandler.h"
#include "Instance/PGGameInstance.h"
#include "Instance/Subsystems/SaveGameManager.h"


void RRender::Init(APGViewport* InViewport, UInputHandler* InInputHandler)
{
	Viewport = InViewport;
	InputHandler = InInputHandler;

	UPGGameInstance* GameInstance = UPGGameInstance::Get();
	assert(GameInstance && "GameInstance was NULLPTR");

	FWindowSettings WindowSettings;
	USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
	if (SaveGameManager)
	{
		WindowSettings = SaveGameManager->RequestUserSettings();
	}
}

void RRender::StartParallel()
{
	RenderThread = std::thread(&RRender::RenderUpdate, this);
}

void RRender::StopWithBlock()
{
	bUpdateRender = false;
	bStop = true;

	if (RenderThread.joinable())
	{
		RenderThread.join();
	}
}

void RRender::CacheRenderData() const
{
	if (!Viewport)
	{
		return;
	}

	NextBuffer.Clear();

	const std::vector<TStrongPtr<APGActor>>& Actors = Viewport->GetViewportActors();
	for (const TStrongPtr<APGActor>& Actor : Actors)
	{
		if (Actor.IsValid())
		{
			if (Actor->GetActorRenderTarget().bRenderOpacity)
			{
				NextBuffer.AddTarget(Actor->GetActorRenderTarget());
			}
			if (Actor->GetActorRenderText().bRenderOpacity)
			{
				NextBuffer.AddTarget(Actor->GetActorRenderText());
			}
		}
	}

	std::lock_guard<std::mutex> Lock(BufferMutex);
	std::swap(Buffer, NextBuffer);
}

void RRender::ClearRenderCache()
{
	Buffer.Clear();
	NextBuffer.Clear();
}

void RRender::RenderUpdate()
{
	while (!bStop)
	{
		while (!bUpdateRender && !bStop)
		{
			std::this_thread::sleep_for(WaitTime);
		}

		if (bStop)
		{
			bStop = false;
			return;
		}

		FRenderData RenderData;
		{
			std::lock_guard<std::mutex> Lock(BufferMutex);
			RenderData = Buffer;
		}

		if (Viewport)
		{
			std::unique_lock<std::shared_mutex>&& RWindowLock = Viewport->LockWindow();

			sf::RenderWindow* RWindow = Viewport->GetRenderWindow();
			if (RWindow && RWindow->isOpen())
			{
				RWindow->setActive(true);

				RWindow->clear();
				for (const FRenderTarget& RTarget : RenderData.Get())
				{
					if (RTarget.Drawable && RTarget.bRenderOpacity)
					{
						RWindow->draw(*RTarget.Drawable, RTarget.Transform);
					}
				}
				RWindow->display();

				RWindow->setActive(false);
			}
		}
	}
}