//
// Utils.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "Utils.h"

void Utils::ShowError(LPCTSTR message, HRESULT result)
{
	_com_error error(result);
	LPCTSTR errorMessage = error.ErrorMessage();

	std::string text;
	text = std::string(message) + "\n\n" + std::string(errorMessage);

	MessageBox(0, text.c_str(), "", 0);
}
