# Console library

A modern C++ wrapper for Windows Console API providing easy-to-use
itarface for console manipulation, text styling, and cursor control.

## Quick Stast
```cpp
#include "console.h"

int main() {
	console cons;
	cons.styleLine("Hello World!", text_color::BrightGreen, bg_color::Blue);
	return 0;
}
```

## Features

– Text Styling 		- Full RGB color support for both text and backgroung
– Viewport Management 	- Control console window size and position
– Buffer Operations 	- Dynamic buffer resizing and management
– Cursor Cinrtol	- Precise cursor positioning and movement
– Prefomance		- Low-level WinAPI calls for maximum efficiency
– Exception Safety	- Comprehensive error handling with meaningful exceptions

## Requirements

– Windows 10/11 or Windows Server 2016+
– C++17 compatible compiler
– Visual Studio 2019+ recommended

## Installation

1. Clone the repository:
```console
git clone https://github.com/thebucagod/Console-Lib.git
```
2. Include in your project:
```console
#include "console.h"
```

## Core methods

StyleLine() 		– Output styled text	– cons.styleLine("Text", color, bg_color)
SetCursorPosition() 	– Move Cursor		– cons.setCursorPosition(x, y);
SetBufferSize() 	– Resize buffer		– cons.setBufferSize(width, height);
SetViewportSize() 	– Resize window		– cons.setViewportSize(width, heaight);

**Warning** – the viewport size shouldn't exceed the buffer size!

## Colors

The enum class is used for colors, and all colors have a code from WinAPI

### Text Colors
```cpp
text_color::Black
text_color::Blue
text_color::Green
text_color::Cyan
text_color::Red
text_color::Magenta
text_color::Yellow
text_color::White
text_color::BrightBlack
text_color::BrightBlue
text_color::BrightGreen
text_color::BrightCyan
text_color::BrightRed
text_color::BrightMagenta
text_color::BrightYellow
text_color::BrightWhite
```

### Background Colors
```cpp
bg_color::Black
bg_color::Blue
bg_color::Green
bg_color::Cyan
bg_color::Red
bg_color::Magenta
bg_color::Yellow
bg_color::White
bg_color::BrightBlack
bg_color::BrightBlue
bg_color::BrightGreen
bg_color::BrightCyan
bg_color::BrightRed
bg_color::BrightMagenta
bg_color::BrightYellow
bg_color::BrightWhite
```