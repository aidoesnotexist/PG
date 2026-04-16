#include "Game/Render.h"
#include "Actor/PGViewport.h"
#include "Actor/PGActor.h"
#include "Core/Components/StrongPtr.h"
#include "Instance/PGGameInstance.h"


void RRender::Init(APGViewport* InViewport)
{
	Viewport = InViewport;

	UPGGameInstance* GameInstance = UPGGameInstance::Get();
	assert(GameInstance && "GameInstance was NULLPTR");
}

void RRender::StartParallel()
{
	RenderThread = std::thread(&RRender::CacheRenderData, this);
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
	while (!bStop)
	{
		while (!bUpdateRender && !bStop)
		{
			std::this_thread::sleep_for(WaitTime);
		}

		if (bStop)
		{
			return;
		}

		if (!Viewport)
		{
			continue;
		}

		FRenderData WorkerBuffer;

		std::vector<TStrongPtr<APGActor>> CopyActors;
		{
			std::lock_guard<std::mutex> lock(Viewport->ActorsMutex);
			CopyActors = Viewport->GetViewportActors();
		}

		for (const TStrongPtr<APGActor>& Actor : CopyActors)
		{
			if (!Actor.IsValid())
			{
				continue;
			}


			std::lock_guard<std::mutex> Lock(Viewport->TargetMutex);
			const FRenderTarget& Target = Actor->GetActorRenderTarget();
			if (Target.bRenderOpacity)
			{
				WorkerBuffer.AddTarget(Target);
			}
			const FRenderTarget& RenderText = Actor->GetActorRenderText();
			if (RenderText.bRenderOpacity)
			{
				WorkerBuffer.AddTarget(RenderText);
			}

		}

		{
			std::lock_guard<std::mutex> Lock(BufferMutex);
			Viewport->NextBuffer = std::move(WorkerBuffer);
			std::swap(Viewport->RenderBuffer, Viewport->NextBuffer);
		}
	}
}

void RRender::ClearRenderCache()
{
	std::lock_guard<std::mutex> Lock(BufferMutex);
	Viewport->RenderBuffer.Clear();
	Viewport->NextBuffer.Clear();
}

std::unique_lock<std::mutex> RRender::LockBuffer() const
{
	std::unique_lock<std::mutex> Lock(BufferMutex);
	return Lock;
}
