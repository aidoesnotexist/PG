#pragma once

#include "BasicTypes.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

class APGViewport;
class UInputHandler;

class RRender
{
public:
	RRender() = default;
	~RRender() = default;

	void Init(APGViewport* InViewport, UInputHandler* InInputHandler);

	void StartParallel();
	void StopWithBlock();

	void CacheRenderData() const;
	void ClearRenderCache();

	std::atomic<bool> bUpdateRender = false;

protected:
	void RenderUpdate();

	std::chrono::milliseconds WaitTime = std::chrono::milliseconds(1000);

private:
	std::thread RenderThread;

	APGViewport* Viewport = nullptr;
	UInputHandler* InputHandler = nullptr;

	mutable std::mutex BufferMutex;
	mutable FRenderData Buffer;
	mutable FRenderData NextBuffer;

	bool bStop = false;
};