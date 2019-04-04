//
// Timer.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// DirectX-Engine (https://github.com/Pindrought/DirectX-Engine)
//

#pragma once

#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	float GetDeltaTime();
	void Update();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_previousTime;
	float fDeltaTime;
};
