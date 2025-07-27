#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include "color.h"

class console {
public:
	console();
	~console();

	// ���������� �����
	bool styleLine(const std::string line, text_color t_col, bg_color b_col);

	// ������ � �������� �������
	void setCursorPosition(const short x, const short y);
	void moveToNextLine(bool line_begin = 0);
	COORD getCursorPosition();


private:

	// �������
	HANDLE _console;
	
	COORD _cursorPosition = { 0, 0 };

};
