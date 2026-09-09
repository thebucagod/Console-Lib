#include "console.h"

#include <stdexcept>

console::console() : _cursorPosition({0, 0}) {
	_console = GetStdHandle(STD_OUTPUT_HANDLE);
	updateConsoleInfo();
}

console::~console() {}


// Viewport
void console::setViewportRECT(const SMALL_RECT& viewportRECT) {
	if (!SetConsoleWindowInfo(_console, true, &viewportRECT)) {
		throw std::runtime_error(
			"Failed to set RECT for console viewport: " +
			std::to_string(GetLastError())
		);
	}

	updateConsoleInfo();
}

void console::setViewportSize(const short width, const short height) {
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

COORD console::getViewportPosition() const {
	return {
	static_cast<short>(_csbi.srWindow.Left),
	static_cast<short>(_csbi.srWindow.Top),
	};
}

COORD console::getViewportSize() const {
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
	if (!SetConsoleScreenBufferSize(_console, size)) {
		throw std::runtime_error(
			std::string("Something went wrong during the resizing!\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	updateConsoleInfo();
}

COORD console::getBufferSize() const {
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
			_console,        // 1. Дескриптор консоли
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
		_console,        // 1. Дескриптор консоли
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
	if (!SetConsoleCursorPosition(_console, _cursorPosition)) {
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

	SetConsoleCursorPosition(_console, _cursorPosition);
}

COORD console::getCursorPosition() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(_console, &csbi)) {
		_cursorPosition = csbi.dwCursorPosition;
	}
	return _cursorPosition;
}

 // Актуализирует все поля структуры _CONSOLE_SCREEN_BUFFER_INFO:
 // dwSize - размеры буфера,
 // dwCursorPosition - Абсолютные координаты курсора,
 // wAttributes - Аттрибуты применимые к текущему отображению данных в буфере,
 // srWindow - Абсолютные координаты верхнего левого и нижнего прававого углов вьюпорта,
 // dwMaximumWindowSize - максимальное возможное значение размеров вьюпорта на данный момент.
bool console::updateConsoleInfo() {
	return GetConsoleScreenBufferInfo(_console, &_csbi);
}

