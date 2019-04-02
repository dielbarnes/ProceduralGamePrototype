//
// Timer.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "Timer.h"

Timer::Timer()
{
	__int64 iCountsPerSecond = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&iCountsPerSecond);
	m_fSecondsPerCount = 1.0f / (float)iCountsPerSecond;

	m_iPreviousTime = iCountsPerSecond;
}

Timer::~Timer()
{
}

void Timer::Update()
{
	__int64 iCurrentTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&iCurrentTime);
	m_fDeltaTime = (iCurrentTime - m_iPreviousTime) * m_fSecondsPerCount;
	m_iPreviousTime = iCurrentTime;
}

float Timer::GetDeltaTime()
{
	return m_fDeltaTime;
}
