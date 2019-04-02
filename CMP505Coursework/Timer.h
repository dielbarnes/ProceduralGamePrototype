//
// Timer.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	void Update();
	float GetDeltaTime();

private:
	float m_fSecondsPerCount;
	__int64 m_iPreviousTime;
	float m_fDeltaTime;
};
