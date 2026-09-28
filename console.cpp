#include "console.h"

#include <stdexcept>

namespace {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO _csbi;

	COORD _minSize = { 50, 1 };
	COORD _cursorPosition = { 0, 0 };

	// Актуализирует все поля структуры _CONSOLE_SCREEN_BUFFER_INFO:
	// dwSize - размеры буфера,
	// dwCursorPosition - Абсолютные координаты курсора,
	// wAttributes - Аттрибуты применимые к текущему отображению данных в буфере,
	// srWindow - Абсолютные координаты верхнего левого и нижнего прававого углов вьюпорта,
	// dwMaximumWindowSize - максимальное возможное значение размеров вьюпорта на данный момент.
	bool updateConsoleInfo() {
		return GetConsoleScreenBufferInfo(hConsole, &_csbi);
	}
}

/*												22 byte
typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
	COORD      dwSize;							contains the size of the console screen buffer, in character columns and rows
	COORD      dwCursorPosition;				contains the column and row coordinates of the cursor in the console screen buffer
	WORD       wAttributes;						The attributes of the characters written to a screen buffer
	SMALL_RECT srWindow;						contains the console screen buffer coordinates of the upper-left and lower-right corners of the display window
	COORD      dwMaximumWindowSize;				contains the maximum size of the console window, in character columns and rows, given the current screen buffer size and font and the screen size.
} CONSOLE_SCREEN_BUFFER_INFO;
*/

// Viewport
void console::setViewportRECT(const SMALL_RECT& viewport) {
	// preliminary validation	
	if (viewport.Left < 0 || viewport.Top < 0 || 
		viewport.Right - viewport.Left + 1 < _minSize.X ||
		viewport.Bottom - viewport.Top + 1 < _minSize.Y) {
		throw std::invalid_argument(
			"Invalid viewport RECT: coordinates are out of logical bounds."
		);
	}

	// buffer compliance check (an updated CSBI is required)
	updateConsoleInfo();

	if (viewport.Right >= _csbi.dwSize.X ||
		viewport.Bottom >= _csbi.dwSize.Y) {
		throw std::invalid_argument(
			std::string("Viewport exceeds buffer size.\n") +
			"Buffer size: " + std::to_string(_csbi.dwSize.X) + "x" + std::to_string(_csbi.dwSize.Y) + "\n" +
			"Viewport Right/Bottom: " + std::to_string(viewport.Right) + "/" + std::to_string(viewport.Bottom)
		);
	}

	// executing a system call
	if (!SetConsoleWindowInfo(hConsole, true, &viewport)) {
		throw std::runtime_error(
			"Failed to set RECT for console viewport: " +
			std::to_string(GetLastError())
		);
	}

	updateConsoleInfo();
}

void console::setViewportSize(const short width, const short height) {
	//an updated CSBI is required
	updateConsoleInfo();

	SMALL_RECT newViewport = _csbi.srWindow;
	newViewport.Right = newViewport.Left + width - 1;
	newViewport.Bottom = newViewport.Top + height - 1;
	setViewportRECT(newViewport);
}

void console::setViewportPosition(const short x, const short y) {
	SMALL_RECT curViewport = _csbi.srWindow;
	short width = curViewport.Right - curViewport.Left + 1;
	short height = curViewport.Bottom - curViewport.Top + 1;

	SMALL_RECT newViewport = {x, y, x + width - 1, y + height - 1};
	setViewportRECT(newViewport);
}

COORD console::getViewportPosition() {
	return {
	static_cast<short>(_csbi.srWindow.Left),
	static_cast<short>(_csbi.srWindow.Top),
	};
}

COORD console::getViewportSize() {
	return {
	static_cast<short>(_csbi.srWindow.Right - _csbi.srWindow.Left + 1),
	static_cast<short>(_csbi.srWindow.Bottom - _csbi.srWindow.Top + 1)
	};
}


// Buffer

void console::setBufferSize(const short width, const short height) {
	// Проверка на выход за границы возможных величин
	if (width > SHRT_MAX || height > SHRT_MAX) {
		throw std::overflow_error(
			std::string("Maximum size error: width or height of buffer is greater than SHRT_MAX.\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	// Проврка минимальных размеров
	if (width < _minSize.X || height < _minSize.Y) {
		throw std::underflow_error(
			std::string("Minimum size error: width or height of buffer is less than _minSize.\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	COORD size = { width, height };
	if (!SetConsoleScreenBufferSize(hConsole, size)) {
		throw std::runtime_error(
			std::string("Something went wrong during the resizing!\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	updateConsoleInfo();
}

COORD console::getBufferSize() {
	return _csbi.dwSize;
}


/* ===== СТИЛИЗАЦИЯ СТРОК ===== */

// Метод для работы с Unicode строками
void console::printStyleLine(const std::wstring &line, text_color t_col, bg_color b_col) {
		const short width = static_cast<short>(line.size());
		const short height = 1;

		COORD bufferSize = { width, height };
		COORD bufferCoord = { 0, 0 };	// Координаты внутри источника, с которых начинается чтение
		SMALL_RECT writeRegion = { _cursorPosition.X, _cursorPosition.Y, _cursorPosition.X + width - 1, _cursorPosition.Y + height - 1 };

		// Массив символов в структуре CHAR_INFO
		std::vector<CHAR_INFO> buffer(line.size());

		WORD text_style = static_cast<WORD>(t_col);
		WORD bg_style = static_cast<WORD>(b_col);

		for (short x = 0; x < width; x++) {
			CHAR_INFO& ci = buffer[x];

			ci.Char.UnicodeChar = line[x];
			ci.Attributes = text_style | bg_style;
		}

		if (!WriteConsoleOutputW(
			hConsole,        // 1. Дескриптор консоли
			buffer.data(),   // 2. Указатель на наш массив CHAR_INFO (источник)
			bufferSize,      // 3. Размер источника
			bufferCoord,     // 4. С какой точки в источнике начинать читать
			&writeRegion     // 5. Указатель на прямоугольник на экране (приемник)
		)) {
			throw std::runtime_error(
				std::string("Something went wrong during the operation (WriteConsoleOutputW())!\n")
			);
		}
	}

// Метод для работы с ANSI строками
void console::printStyleLine(const std::string& line, text_color t_col, bg_color b_col) {
	const short width = static_cast<short>(line.size());
	const short height = 1;

	COORD bufferSize = { width, height };
	COORD bufferCoord = { 0, 0 };	// Координаты внутри источника, с которых начинается чтение
	SMALL_RECT writeRegion = { _cursorPosition.X, _cursorPosition.Y, _cursorPosition.X + width - 1, _cursorPosition.Y + height - 1 };

	// Массив символов в структуре CHAR_INFO
	std::vector<CHAR_INFO> buffer(line.size());

	WORD text_style = static_cast<WORD>(t_col);
	WORD bg_style = static_cast<WORD>(b_col);

	for (short x = 0; x < width; x++) {
		CHAR_INFO& ci = buffer[x];

		ci.Char.AsciiChar = line[x];
		ci.Attributes = text_style | bg_style;
	}

	if (!WriteConsoleOutputA(
		hConsole,        // 1. Дескриптор консоли
		buffer.data(),   // 2. Указатель на наш массив CHAR_INFO (источник)
		bufferSize,      // 3. Размер источника
		bufferCoord,     // 4. С какой точки в источнике начинать читать
		&writeRegion     // 5. Указатель на прямоугольник на экране (приемник)
	)) {
		throw std::runtime_error(
			std::string("Something went wrong during the operation (WriteConsoleOutputW())!\n")
		);
	}
}

// Работа с курсором консоли

void console::setCursorPosition(const short x, const short y) {
	// Проверка на минимальные и максимальные размеры.
	if (x < 0 || y < 0 || x > SHRT_MAX || y > SHRT_MAX) {
		throw std::out_of_range(
			std::string("Cursor position is out of valid SHORT range!\n") +
			"X: " + std::to_string(x) + '\n' +
			"Y: " + std::to_string(y) + '\n'
		);
	}	

	// Номер столбца
	_cursorPosition.X = x;
	// Номер строки
	_cursorPosition.Y = y;

	// Применяем новые координаты WinAPI методом SetConsoleCursorPosition()
	if (!SetConsoleCursorPosition(hConsole, _cursorPosition)) {
		throw std::runtime_error(
			std::string("Something went wrong during the operation (SetConsoleCursorPosition)!\n") +
			"X: " + std::to_string(x) + '\n' +
			"Y: " + std::to_string(y) + '\n'
		);
	}
}

void console::moveToNextLine(bool line_begin) {
	if (line_begin)
		_cursorPosition.X = 0;
	
	_cursorPosition.Y += 1;

	SetConsoleCursorPosition(hConsole, _cursorPosition);
}

COORD console::getCursorPosition() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		_cursorPosition = csbi.dwCursorPosition;
	}
	return _cursorPosition;
}