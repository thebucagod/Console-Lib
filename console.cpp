#include "console.h"

#include <stdexcept>

console::console() : _cursorPosition({0, 0}) {
	_console = GetStdHandle(STD_OUTPUT_HANDLE);
	updateConsoleInfo();
}

console::~console() {}

// Viewport

//void console::setViewportSize(const short width, const short height) {
//	
//}

COORD console::getViewportSize() {
	return {
	static_cast<short>(_csbi.srWindow.Right - _csbi.srWindow.Left + 1),
	static_cast<short>(_csbi.srWindow.Bottom - _csbi.srWindow.Top + 1)
	};
}


// Buffer

void console::setBufferSize(const short width, const short height) {
	if (width > SHRT_MAX || height > SHRT_MAX) {
		throw std::overflow_error(
			std::string("Maximum size error: width or height is greater than SHRT_MAX.\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	//Проврка минимальных размеров
	if (width < _minSize.X || height < _minSize.Y) {
		throw std::underflow_error(
			std::string("Minimum size error: width or height is less than _minSize.\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	COORD size = { width, height };
	if (!SetConsoleScreenBufferSize(_console, size)) {
		throw std::runtime_error(
			std::string("Something went wrong during the resizing!\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	if (!updateConsoleInfo()) {
		throw std::runtime_error(
			std::string("Something went wrong while changing the console information!\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}
}

COORD console::getBufferSize() {
	return _csbi.dwSize;
}


// Стилизация строк

bool console::styleLine(const std::string &line, text_color t_col, bg_color b_col) {
	const short width = static_cast<short>(line.size());
	const short height = 1;

	COORD bufferSize = { width, height };
	COORD bufferCoord = { 0, 0 };
	SMALL_RECT writeRegion = { _cursorPosition.X, _cursorPosition.Y, _cursorPosition.X + width - 1, _cursorPosition.Y + height - 1 };

	std::vector<CHAR_INFO> buffer(line.size());

	WORD text_style = static_cast<WORD>(t_col);
	WORD bg_style = static_cast<WORD>(b_col);

	for (short x = 0; x < width; x++) {
		CHAR_INFO& ci = buffer[x];

		ci.Char.AsciiChar = line[x];
		ci.Attributes = text_style | bg_style;
	}

	WriteConsoleOutput(
		_console,
		buffer.data(),
		bufferSize,
		bufferCoord,
		&writeRegion
	);

	moveToNextLine();
	
	return false;
}


// Работа с курсором консоли

void console::setCursorPosition(const short x, const short y) {
	_cursorPosition.X = x;
	_cursorPosition.Y = y;

	SetConsoleCursorPosition(_console, _cursorPosition);
}

void console::moveToNextLine(bool line_begin) {
	if (line_begin)
		_cursorPosition.X = 0;
	
	_cursorPosition.Y += 1;

	SetConsoleCursorPosition(_console, _cursorPosition);
}

COORD console::getCursorPosition() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(_console, &csbi)) {
		_cursorPosition = csbi.dwCursorPosition;
	}
	return _cursorPosition;
}

bool console::updateConsoleInfo() {
	return GetConsoleScreenBufferInfo(_console, &_csbi);
}



