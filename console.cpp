#include "console.h"

console::console() : _cursorPosition({0, 0}) {
	_console = GetStdHandle(STD_OUTPUT_HANDLE);
	updateConsoleInfo();
}

console::~console() {}

// Viewport

COORD console::getViewportSize() {
	return {
	static_cast<short>(_csbi.srWindow.Right - _csbi.srWindow.Left + 1),
	static_cast<short>(_csbi.srWindow.Bottom - _csbi.srWindow.Top + 1)
	};
}


// Buffer

bool console::setBufferSize(short width, short height) {
	//Проврка минимальных размеров
	width = max(width, _minSize.X);
	height = max(height, _minSize.Y);
	
	if (width > SHRT_MAX || height > SHRT_MAX) {
		return false;
	}

	COORD size = { width, height };
	if (!SetConsoleScreenBufferSize(_console, size)) {
		return false;
	}

	if (updateConsoleInfo()) {
		return true;
	}
	return false;
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



