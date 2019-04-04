//
// Timer.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//
// Reference:
// DirectX-Engine (https://github.com/Pindrought/DirectX-Engine)
//

#include "Timer.h"

Timer::Timer()
{
	m_previousTime = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
}

float Timer::GetDeltaTime()
{
	return fDeltaTime;
}

void Timer::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto elapsedMilliseconds = std::chrono::duration<float, std::milli>(currentTime - m_previousTime);
	fDeltaTime = elapsedMilliseconds.count();
	m_previousTime = currentTime;
}
