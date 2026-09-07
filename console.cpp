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
/// @throws std::overflow_error Если переданные width или height больше SHRT_MAX.
/// @throws std::underflow_error Если переданные width или height меньше _minSize.
/// @throws std::runtime_error Если системный вызов SetConsoleScreenBufferSize()
/// внутри данного метода завершился с ошибкой.
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

/// @brief Возвращает текущее количество строк и столбцов буфера, type COORD.
/// @return Структура COORD, где поле X содержит количество столбцов,
///			а поле Y - количество строк.
/// @note Значения рассчитываются на основе кэшированного состояние (_csbi).
COORD console::getBufferSize() const {
	return _csbi.dwSize;
}


// Стилизация строк

/// @brief Выводит строку на текущее положение курсора, с заданными цветом символов и фона.
/// @details Отображение принимаемых данных в буфере консоли.
/// Необходимо для вывода стилизованных строк.
/// @param line строка-данные для записи в консоль.
/// @param t_col цвет текста строки (text_color::White по умолчанию).
/// @param b_col цвет текста фона строки (bg_color::Black по умолчанию).
/// @note Вывод выполняется через WinAPI метод WriteConsoleOutput.
bool console::printStyleLine(const std::string &line, text_color t_col, bg_color b_col) {
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
			
		return false;
	}


// Работа с курсором консоли

/// @brief Устанавливает курсор на передоваему позицию.
/// @note Установка курсора выполняется через WinAPI метод SetConsolePosition()
/// @throws ...
void console::setCursorPosition(const short x, const short y) {
	_cursorPosition.X = x;
	_cursorPosition.Y = y;

	SetConsoleCursorPosition(_console, _cursorPosition);
}

/// @brief Устанавливает курсор на следующую строку.
/// @note Установка курсора выполняется через WinAPI метод SetConsolePosition().
/// @param line_begin 0 - если курсор должен остаться на том же столбце
///					  1 - если курсор должен уйти в начало строки (на нулевой столбец)
/// @throws ...
void console::moveToNextLine(bool line_begin) {
	if (line_begin)
		_cursorPosition.X = 0;
	
	_cursorPosition.Y += 1;

	SetConsoleCursorPosition(_console, _cursorPosition);
}

/// @brief Возвращает COORD текущего положения курсора в символьных столбцах и строках (начиная с 0)
/// @return Структура COORD, где поле X содержит текущий столбец,
///			а поле Y - текущая строка.
COORD console::getCursorPosition() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(_console, &csbi)) {
		_cursorPosition = csbi.dwCursorPosition;
	}
	return _cursorPosition;
}

/// @brief Актуализирует данные о консоли
/// @details Актуализирует все поля структуры _CONSOLE_SCREEN_BUFFER_INFO:
/// dwSize - размеры буфера,
/// dwCursorPosition - Абсолютные координаты курсора,
/// wAttributes - Аттрибуты применимые к текущему отображению данных в буфере,
/// srWindow - Абсолютные координаты верхнего левого и нижнего прававого углов вьюпорта,
/// dwMaximumWindowSize - максимальное возможное значение размеров вьюпорта на данный момент.
/// @warning Пользователь может самостоятельно изменять размер вьюпорта,
/// эти данные в классе на данный момент никак не регулируются автоматически.
/// Понимаю, что нельзя каждый раз предугадать поведение пользователя, но по возможности рекомендуется использовать данный метод
/// при значимых изменениях консоли для актуализации данных
bool console::updateConsoleInfo() {
	return GetConsoleScreenBufferInfo(_console, &_csbi);
}

