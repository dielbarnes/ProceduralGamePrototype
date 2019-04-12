//
// Utils.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "Utils.h"

void Utils::ShowError(LPCTSTR message, HRESULT result)
{
	_com_error error(result);
	std::string text = std::string(message) + "\n\n" + std::string(error.ErrorMessage());
	MessageBox(0,				// Owner window of the message box
			   text.c_str(), 
			   "",				// Title
			   0);				// Buttons to be displayed					
}

std::string Utils::GetDirectoryFromPath(std::string strFilePath)
{
	size_t backslashOffset = strFilePath.find_last_of('\\');
	size_t slashOffset = strFilePath.find_last_of('/');

	if (backslashOffset == std::string::npos && slashOffset == std::string::npos)
	{
		return "";
	}
	else if (backslashOffset == std::string::npos)
	{
		return strFilePath.substr(0, slashOffset);
	}
	else if (slashOffset == std::string::npos)
	{
		return strFilePath.substr(0, backslashOffset);
	}
	
	return strFilePath.substr(0, max(backslashOffset, slashOffset));
}

std::string Utils::GetFileExtension(std::string strFilename)
{
	size_t offset = strFilename.find_last_of('.');
	if (offset == std::string::npos)
	{
		return {}; // Return an empty string
	}
	return std::string(strFilename.substr(offset + 1));
}
