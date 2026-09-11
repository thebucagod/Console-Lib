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

	/// @brief Sets the position and size of the console viewport.
	/// @details Applise the scpecified coordinates using the WinAPI. 
	/// On succes, it automatically updates the internal console information cache.
	/// @param viewport - A SMALL_RECT structure specifying the new window bounds.
	/// @throws std::runtime_error - If the SetConsoleWindowInfo() system call fails.
	void setViewportRECT(const SMALL_RECT& viewport);

	/// @brief Resizes the console viewport
	/// @details This method adjusts the viewport size while maintaining a fixed 
	/// position of its top-left corner. It is used it is necessary to expand or shrink
	/// the visible working area without shifting the current content relative to the origin.
	/// @param width - The new width of the viewport.
	/// @param height - The new height of the viewport.
	void setViewportSize(const short width, const short height);

	/// @brief Shifts the console viewport to the specified coordinates.
	/// @details This method moves the viewport while sctrictly pressing its 
	/// current dimensions.
	/// It is used to implement scrolling of the console butter content
	/// without altering the display scale.
	/// @param x - The new X coordinate of the top-left.
	/// @param y - The new Y coordinate of the top-left.
	void setViewportPosition(const short x, const short y);

	/// @brief Returns the top-left corner coordinates of the viewport.
	/// @return a COORD structre where the X field contatins the horizontal position,
	///			and the Y field contatins the fertical position
	/// (relative to the console buffre origin)
	COORD getViewportPosition() const;

	/// @brief Returns the currents dimensions of the current viewport.
	/// @return A COORD strcutere whree the X fields contatins the width,
	///			and the Y field contains the height og the area in characters.
	/// @note the values are derived from the cached state (_csbi).
	/// @warning If the user manually resizes the viewport,
	/// the data may become inaccurate.
	COORD getViewportSize() const;

	// Buffer

	/// @brief Resizes the console buffer.
	/// @details This method adjusts the buffer dimensions.
	/// It sets the new dimensions in character columns and rows.
	/// It is used when the size of the data-displaying buffer needs to be changed.
	/// @param width The new number of buffer columns.
	/// @param height The new number of buffer rows.
	/// @throws std::overflow_error If the provided width or height exceeds SHRT_MAX.
	/// @throws std::underflow_error If the provided width or height is less than _minSize.
	/// @throws std::runtime_error If the SetConsoleScreenBufferSize() system call
	/// within this method fails.
	void setBufferSize(const short width, const short height);

	/// @brief Returns the current number of buffer rows and columns as a COORD type.
	/// @return A COORD structure where the X field contains the number of columns,
	///         and the Y field contains the number of rows.
	/// @note The values are derived from the cached state (_csbi).
	COORD getBufferSize() const;

	// Line styling

	/// @brief Outputs a Unicode string at the current cursor position with specified colors.
	/// @details This is the recommended method for text output. It guarantees correct
	/// rendering of Cyrillic, box-drawing characters, and special symbols regardless of
	/// the user's operating system language settings.
	/// @param line The wide string (std::wstring) to be written to the console.
	/// @param t_col The text color (default: text_color::White).
	/// @param b_col The background color (default: bg_color::Black).
	/// @note Internally uses the WinAPI WriteConsoleOutputW method.
	void printStyleLine(const std::wstring& line, text_color t_col = text_color::White, bg_color b_col = bg_color::Black);

	/// @brief Outputs an ANSI string at the current cursor position with specified colors.
	/// @details Provides maximum performance due to ANSI encoding and the absence of conversion.
	/// @param line The narrow string (std::string) to be written to the console.
	/// @param t_col The text color (default: text_color::White).
	/// @param b_col The background color (default: bg_color::Black).
	/// @warning Use this method **only for Latin characters and basic ASCII symbols**. 
	/// Outputting Cyrillic via this method depends on the system code page (CP_ACP) and 
	/// will guaranteedly result in unreadable characters on computers 
	/// with non-Russian Windows language settings.
	/// @note Internally uses the WinAPI WriteConsoleOutputA method.
	void printStyleLine(const std::string& line, text_color t_col = text_color::White, bg_color b_col = bg_color::Black);

	// Console cursor operations

	/// @brief Sets the cursor to the specified position.
	/// @note Cursor positioning is performed via the WinAPI SetConsoleCursorPosition() method.
	/// @throws std::out_of_range If the provided X or Y is greater than SHRT_MAX or less than 0.
	/// @throws std::runtime_error If the SetConsoleCursorPosition() system call fails.
	void setCursorPosition(const short x, const short y);

	/// @brief Moves the cursor to the next line.
	/// @note Cursor positioning is performed via the WinAPI SetConsoleCursorPosition() method.
	/// @param line_begin 0 - if the cursor should remain in the same column,
	///                   1 - if the cursor should move to the beginning of the line (column 0).
	void moveToNextLine(bool line_begin = 0);

	/// @brief Returns the COORD of the current cursor position in character columns and rows (starting from 0).
	/// @return A COORD structure where the X field contains the current column,
	///         and the Y field contains the current row.
	COORD getCursorPosition();


private:
	// Updates _csbi.
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
