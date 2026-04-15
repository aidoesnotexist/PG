#pragma once

#include "Core/Components/Ticker.h"

void TTicker::Start()
{
	bIsTicking = true;
	Delta = std::chrono::steady_clock::now();
}

void TTicker::Reset()
{
	Delta = std::chrono::steady_clock::now();
}

float TTicker::GetElapsedTime()
{
	/*using namespace std::chrono;

	const auto Now = steady_clock::now();
	float DeltaTime = duration<float>(Now - Delta).count();

	if (DeltaTime > 0.05f)
	{
		DeltaTime = 0.05f;
	}

	Delta = Now;*/
	using namespace std::chrono;

	const auto Now = steady_clock::now();

	float DeltaTime = duration<float>(Now - Delta).count();

	Delta = Now;

	//return DeltaTime;

	return DeltaTime;
}