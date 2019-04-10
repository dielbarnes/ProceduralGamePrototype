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
