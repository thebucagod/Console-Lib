#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>

#include "color.h"

class console {
public:
	console();
	~console();

	// Viewport
	void setViewportSize(const short width, const short height);
	void setViewportPosition(const short x, const short y);
	COORD getViewportPosition();
	COORD getViewportSize();

	// Buffer
	void setBufferSize(const short width, const short height);
	COORD getBufferSize();

	// Стилизация строк
	bool styleLine(const std::string &line, text_color t_col, bg_color b_col);

	// Работа с курсором консоли
	void setCursorPosition(const short x, const short y);
	void moveToNextLine(bool line_begin = 0);
	COORD getCursorPosition();


private:
	// Проверка размеров
	bool updateConsoleInfo();


	HANDLE _console;
	COORD _cursorPosition = { 0, 0 };
	COORD _minSize = { 80, 25 };
	CONSOLE_SCREEN_BUFFER_INFO _csbi;
	
	/*
	typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
		COORD      dwSize;							contains the size of the console screen buffer, in character columns and rows
		COORD      dwCursorPosition;				contains the column and row coordinates of the cursor in the console screen buffer
		WORD       wAttributes;						The attributes of the characters written to a screen buffer
		SMALL_RECT srWindow;						contains the console screen buffer coordinates of the upper-left and lower-right corners of the display window
		COORD      dwMaximumWindowSize;				contains the maximum size of the console window, in character columns and rows, given the current screen buffer size and font and the screen size.
	} CONSOLE_SCREEN_BUFFER_INFO;
	*/
};
