#include "console.h"

#include <stdexcept>

console::console() : _cursorPosition({0, 0}) {
	_console = GetStdHandle(STD_OUTPUT_HANDLE);
	updateConsoleInfo();
}

console::~console() {}


// Viewport

/// @brief Задает положение и размер видимой области (viewport) консоли.
/// @details Применяет заданные координаты через WinAPI. 
/// В случае успеха автоматически обновляет внутренний кэш информации о консоли.
/// @param viewport Структура SMALL_RECT, задающая новые границы окна.
/// @throws std::runtime_error Если системный вызов SetConsoleWindowInfo завершился с ошибкой.
void console::setViewportRECT(const SMALL_RECT& viewportRECT) {
	if (!SetConsoleWindowInfo(_console, true, &viewportRECT)) {
		throw std::runtime_error(
			"Failed to set RECT for console viewport: " +
			std::to_string(GetLastError())
		);
	}

	updateConsoleInfo();
}

/// @brief Изменяет размеры видимой области (viewport) консоли
/// @details Метод масштабирует видимую область, сохраняя фиксированную позицию
/// её левого верхнего угла. Используется, когда необходимо расширить или сузить 
/// рабочую зону отображения без смещения текущего содержимого относительно начала координат.
/// @param width Новая ширина видимой области.
/// @param height Новая высота видимой области.
void console::setViewportSize(const short width, const short height) {
	SMALL_RECT newViewport = _csbi.srWindow;
	newViewport.Right = newViewport.Left + width - 1;
	newViewport.Bottom = newViewport.Top + height - 1;
	setViewportRECT(newViewport);
}

/// @brief Смещает видимую область (viewport) консоли в заданные координаты.
/// @details Метод перемещает видимую область, строго сохроняя её текущие размеры.
/// Используется для реализации прокрутки (скроллинга) содержимого буфера консоли
/// без изменения масштаба изображения.
/// @param x Новая координата X левого верхнего угла.
/// @param y Новая координата Y левого верхнего угла.
void console::setViewportPosition(const short x, const short y) {
	SMALL_RECT curViewport = _csbi.srWindow;
	short width = curViewport.Right - curViewport.Left + 1;
	short height = curViewport.Bottom - curViewport.Top + 1;

	SMALL_RECT newViewport = {x, y, x + width - 1, y + height - 1};
	setViewportRECT(newViewport);
}
 
/// @brief Возвращает координаты левого верхнего угла видимой области.
/// @return Структура COORD, где поле X содержит позицию по горизонтали,
///			а поле Y - по вертикали (относительно начала буфера консоли).
COORD console::getViewportPosition() const {
	return {
	static_cast<short>(_csbi.srWindow.Left),
	static_cast<short>(_csbi.srWindow.Top),
	};
}

/// @brief Возвращает текущие размеры видимой области консоли.
/// @return Структура COORD, где поле X содержит ширину,
///			а поле Y - высоту области в символах.
/// @note Значения рассчитываются на основе кэшированного состояние (_csbi).
COORD console::getViewportSize() const {
	return {
	static_cast<short>(_csbi.srWindow.Right - _csbi.srWindow.Left + 1),
	static_cast<short>(_csbi.srWindow.Bottom - _csbi.srWindow.Top + 1)
	};
}


// Buffer

/// @brief Изменяет размер буфера (buffer).
/// @details Метод масштабирует размер буфера.
/// Задаёт новые размеры в символьлных стоблцах и строках.
/// Используется когда необходимо изменить размеры буфера, отображающий данные.
/// @param width Новое количество столбцов буфера.
/// @param height Новое количество строк буфера.
void console::setBufferSize(const short width, const short height) {
	// Проверка на выход за границы возможных величин
	if (width > SHRT_MAX || height > SHRT_MAX) {
		throw std::overflow_error(
			std::string("Maximum size error: width or height is greater than SHRT_MAX.\n") +
			"width: " + std::to_string(width) + '\n' +
			"height: " + std::to_string(height) + '\n'
		);
	}

	// Проврка минимальных размеров
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

	updateConsoleInfo();
}

/// @brief Возвращает текущее количество строк и столбцов буфера, type COORD.
/// @return Структура COORD, где поле X содержит количество столбцов,
///			а поле Y - количество строк.
/// @note Значения рассчитываются на основе кэшированного состояние (_csbi).
COORD console::getBufferSize() {
	return _csbi.dwSize;
}


// Стилизация строк

bool console::styleLine(const std::string &line, text_color t_col, bg_color b_col) {
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

		WriteConsoleOutput(
			_console,        // 1. Дескриптор консоли
			buffer.data(),   // 2. Указатель на наш массив CHAR_INFO (источник)
			bufferSize,      // 3. Размер источника
			bufferCoord,     // 4. С какой точки в источнике начинать читать
			&writeRegion     // 5. Указатель на прямоугольник на экране (приемник)
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



