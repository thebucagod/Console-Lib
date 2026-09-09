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

	/// @brief Задает положение и размер видимой области (viewport) консоли.
	/// @details Применяет заданные координаты через WinAPI. 
	/// В случае успеха автоматически обновляет внутренний кэш информации о консоли.
	/// @param viewport Структура SMALL_RECT, задающая новые границы окна.
	/// @throws std::runtime_error Если системный вызов SetConsoleWindowInfo завершился с ошибкой.
	void setViewportRECT(const SMALL_RECT& viewport);

	/// @brief Изменяет размеры видимой области (viewport) консоли
	/// @details Метод масштабирует видимую область, сохраняя фиксированную позицию
	/// её левого верхнего угла. Используется, когда необходимо расширить или сузить 
	/// рабочую зону отображения без смещения текущего содержимого относительно начала координат.
	/// @param width Новая ширина видимой области.
	/// @param height Новая высота видимой области.
	void setViewportSize(const short width, const short height);

	/// @brief Смещает видимую область (viewport) консоли в заданные координаты.
	/// @details Метод перемещает видимую область, строго сохроняя её текущие размеры.
	/// Используется для реализации прокрутки (скроллинга) содержимого буфера консоли
	/// без изменения масштаба изображения.
	/// @param x Новая координата X левого верхнего угла.
	/// @param y Новая координата Y левого верхнего угла.
	void setViewportPosition(const short x, const short y);

	/// @brief Возвращает координаты левого верхнего угла видимой области.
	/// @return Структура COORD, где поле X содержит позицию по горизонтали,
	///			а поле Y - по вертикали (относительно начала буфера консоли).
	COORD getViewportPosition() const;

	/// @brief Возвращает текущие размеры видимой области консоли.
	/// @return Структура COORD, где поле X содержит ширину,
	///			а поле Y - высоту области в символах.
	/// @note Значения рассчитываются на основе кэшированного состояние (_csbi).
	/// @warning Если пользователь самостоятельно изменит размер вьюпорта тогда
	/// данные могут быть не точны.
	COORD getViewportSize() const;

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
	void setBufferSize(const short width, const short height);

	/// @brief Возвращает текущее количество строк и столбцов буфера, type COORD.
	/// @return Структура COORD, где поле X содержит количество столбцов,
	///			а поле Y - количество строк.
	/// @note Значения рассчитываются на основе кэшированного состояние (_csbi).
	COORD getBufferSize() const;

	// Стилизация строк

	/// @brief Выводит Unicode-строку на текущее положение курсора, с заданными цветами.
	/// @details Рекомендуемый метод для вывода текста. Он гарантирует корректное
	/// отображение кириллицы, псевдографики и специальных символов независимо от
	/// языковых настроек операционной системы пользователя.
	/// @param line Широкая строка (std::wstring) для записи в консоль.
	/// @param t_col цвет текста (по умолчанию text_color::White).
	/// @param b_col цвет текста (по умолчанию bg_color::Black).
	/// @note Внутри используется WinAPI метод WriteConsoleOutputW.
	void printStyleLine(const std::wstring& line, text_color t_col = text_color::White, bg_color b_col = bg_color::Black);

	/// @brief Выводит ANSI-строку на текущее положение курсора, с заданными цветами.
	/// @details Обеспечивает максимальную производительность за счёт ANSI кодировки и отсутвия конвертации.
	/// @param line Узкая строка (std::string) для записи в консоль.
	/// @param t_col цвет текста (по умолчанию text_color::White).
	/// @param b_col цвет текста (по умолчанию bg_color::Black).
	/// @warning Используйте этот метод **только для латиницы и базовых ASCII-символов**. 
	/// Вывод кириллицы через этот метод зависит от системной кодовой страницы (CP_ACP) и 
	/// гарантированно приведет к отображению нечитаемых символов на компьютерах 
	/// с нерусскими языковыми настройками Windows.
	/// @note Внутри используется WinAPI метод WriteConsoleOutputA.
	void printStyleLine(const std::string& line, text_color t_col = text_color::White, bg_color b_col = bg_color::Black);

	// Работа с курсором консоли

	/// @brief Устанавливает курсор на передоваему позицию.
	/// @note Установка курсора выполняется через WinAPI метод SetConsolePosition().
	/// @throws std::out_of_range Если переданные X или Y больше SHRT_MAX или меньше 0.
	/// @throws std::runtime_error Если системный вызов SetConsoleCursorPosition().
	void setCursorPosition(const short x, const short y);

	/// @brief Устанавливает курсор на следующую строку.
	/// @note Установка курсора выполняется через WinAPI метод SetConsolePosition().
	/// @param line_begin 0 - если курсор должен остаться на том же столбце
	///					  1 - если курсор должен уйти в начало строки (на нулевой столбец)
	void moveToNextLine(bool line_begin = 0);

	/// @brief Возвращает COORD текущего положения курсора в символьных столбцах и строках (начиная с 0)
	/// @return Структура COORD, где поле X содержит текущий столбец,
	///			а поле Y - текущая строка.
	COORD getCursorPosition();


private:
	// Обновление _csbi	.
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
