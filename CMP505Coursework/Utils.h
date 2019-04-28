//
// Utils.h
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#pragma once

#include <winerror.h>
#include <comdef.h> 
#include <string>
#include <random>

#define COLOR_F4(r, g, b, a)	{ r/255.0f, g/255.0f, b/255.0f, a };
#define COLOR_XMF4(r, g, b, a)	XMFLOAT4(r/255.0f, g/255.0f, b/255.0f, a)

#define SAFE_RELEASE(p)			if (p) p->Release();
#define SAFE_DELETE(p)			if (p) { delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY(p)	if (p) { delete[] p; p = nullptr; }

class Utils
{
public:
	static void ShowError(LPCTSTR message, HRESULT result);
	static std::string GetDirectoryFromPath(std::string strFilePath);
	static std::string GetFileExtension(std::string strFilename);
	static int GetRandomInt(int iMin, int iMax);

private:
	static std::random_device randomDevice;
	static std::mt19937_64 randomNumberEngine;
	static std::uniform_int_distribution<int> dist;
};
