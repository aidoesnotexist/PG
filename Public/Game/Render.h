#pragma once

#include "BasicTypes.h"
#include "Actor/PGViewport.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

/*
	Initially was meant to be a real renderer that handles parallel drawing
	actors render targets. Since SFML didn't show itself good with the multithreading
	window usage, it was downgraded to just paralel buffers calculation
*/
class RRender
{
	friend class APGViewport;

public:
	RRender() = default;
	~RRender() = default;

	void Init(APGViewport* InViewport);

	void StartParallel();
	void StopWithBlock();

	void ClearRenderCache();

	std::atomic<bool> bUpdateRender = false;

protected:
	void CacheRenderData() const;

	std::chrono::milliseconds WaitTime = std::chrono::milliseconds(1000);

private:
	std::thread RenderThread;

	std::unique_lock<std::mutex> LockBuffer() const;

	APGViewport* Viewport = nullptr;

	mutable std::mutex BufferMutex;

	bool bStop = false;
};