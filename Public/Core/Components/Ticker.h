#pragma once

#include <chrono>

class TTicker
{
public:
	TTicker() : bIsTicking(), Delta(std::chrono::milliseconds(0))
	{
	}
	~TTicker() = default;

	void Start();
	void Reset();

	float GetElapsedTime();
	bool IsTicking() const { return bIsTicking; }

private:
	bool bIsTicking;
	std::chrono::steady_clock::time_point Delta;
};