// basic_example.cpp
//
// A short tour of the console library.
//
// Build (MSVC, Developer Command Prompt):
//     cl /EHsc /std:c++17 /utf-8 basic_example.cpp console.cpp
//
// The /utf-8 flag is required so that string literals containing
// non-ASCII characters are stored as UTF-8, which is what the library expects.

#include <iostream>
#include <string>
#include <exception>

#include "console.h"

int main() {
	try {
		// ------------------------------------------------------------
		// 1. Simple output
		// ------------------------------------------------------------
		// printStyleLine() prints a string and then moves the cursor to the
		// beginning of the next line. Without color arguments it uses
		// white text on a black background.
		console::printStyleLine("1. Simple output");
		console::printStyleLine("Hello, console!");
		console::moveToNextLine(true); // an empty line as a separator

		// ------------------------------------------------------------
		// 2. Colors
		// ------------------------------------------------------------
		// The second argument is the text color, the third is the background color.
		console::printStyleLine("2. Colors");
		console::printStyleLine("Bright red text", text_color::BrightRed);
		console::printStyleLine("Black text on a yellow background", text_color::Black, bg_color::Yellow);
		console::printStyleLine("Bright white text on a blue background", text_color::BrightWhite, bg_color::Blue);
		console::moveToNextLine(true);

		// ------------------------------------------------------------
		// 3. Several styles on a single line
		// ------------------------------------------------------------
		// printStyle() does NOT move to the next line. After it finishes, the
		// cursor stands right after the last printed character, so consecutive
		// calls continue on the same line.
		console::printStyleLine("3. Several styles on one line");
		console::printStyle("[ OK ]", text_color::BrightGreen);
		console::printStyle(" ");
		console::printStyle("[WARN]", text_color::BrightYellow);
		console::printStyle(" ");
		console::printStyle("[FAIL]", text_color::BrightRed);
		console::moveToNextLine(true); // finish the line manually
		console::moveToNextLine(true);

		// ------------------------------------------------------------
		// 4. Unicode
		// ------------------------------------------------------------
		// Strings are treated as UTF-8 and converted to UTF-16 internally.
		// Single-width characters from the Basic Multilingual Plane work fine:
		// Cyrillic, box-drawing characters, block elements, and so on.
		console::printStyleLine("4. Unicode");
		console::printStyleLine("Привет, мир!", text_color::BrightCyan);
		console::printStyleLine("┌──────────┐", text_color::Cyan);
		console::printStyleLine("│ ░▒▓██▓▒░ │", text_color::Cyan);
		console::printStyleLine("└──────────┘", text_color::Cyan);
		console::moveToNextLine(true);

		// ------------------------------------------------------------
		// 5. Working with the cursor
		// ------------------------------------------------------------
		// Coordinates are absolute and zero-based: (0, 0) is the top-left cell
		// of the console BUFFER (not of the visible window).
		console::printStyleLine("5. Cursor control");

		COORD saved = console::getCursorPosition(); // remember where we are

		console::setCursorPosition(30, saved.Y);    // jump to column 30 of the same row
		console::printStyle("<- printed at column 30", text_color::BrightMagenta);

		console::setCursorPosition(0, saved.Y);     // jump back to the start of the row
		console::printStyle("Text at column 0", text_color::BrightMagenta);

		console::moveToNextLine(true);
		console::moveToNextLine(true);

		// ------------------------------------------------------------
		// 6. Truncation at the right edge of the buffer
		// ------------------------------------------------------------
		// If a string does not fit between the cursor and the right edge of the
		// buffer, the part that does not fit is discarded (no automatic wrapping).
		// The cursor is left on the last column of the buffer.
		console::printStyleLine("6. Truncation");

		COORD bufferSize = console::getBufferSize();
		std::string tooLong(static_cast<size_t>(bufferSize.X) + 20, '#');

		console::printStyle(tooLong, text_color::BrightBlack);
		console::moveToNextLine(true); // move on to the next line manually

		console::printStyleLine(
			"The line above was longer than the buffer, so it was cut off.",
			text_color::BrightYellow
		);
		console::moveToNextLine(true);

		// ------------------------------------------------------------
		// 7. Information about the console
		// ------------------------------------------------------------
		console::printStyleLine("7. Console information");

		COORD viewportSize = console::getViewportSize();
		COORD cursor = console::getCursorPosition();

		console::printStyleLine(
			"Buffer size:   " + std::to_string(bufferSize.X) + " x " + std::to_string(bufferSize.Y)
		);
		console::printStyleLine(
			"Viewport size: " + std::to_string(viewportSize.X) + " x " + std::to_string(viewportSize.Y)
		);
		console::printStyleLine(
			"Cursor:        (" + std::to_string(cursor.X) + ", " + std::to_string(cursor.Y) + ")"
		);
		console::moveToNextLine(true);

		console::printStyleLine("Press Enter to exit...", text_color::BrightWhite);
	}
	catch (const std::exception &e) {
		// All library errors are reported as standard exceptions
		// (std::runtime_error, std::out_of_range, std::invalid_argument, ...).
		std::cerr << "Console error: " << e.what() << std::endl;
		return 1;
	}

	std::cin.get();
	return 0;
}
