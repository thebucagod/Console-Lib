#include <windows.h>
#include <vector>
#include <string>
#include <iostream>

#include "console.h"
#include "color.h"

int main() {
	console cons;
    	
    // ������� ������ � �����������������
    cons.setCursorPosition(10, 5);
    cons.styleLine("Centered text", text_color::BrightYellow, bg_color::Blue);

    // ������� ��������� ����� ������
    cons.styleLine("First line", text_color::BrightWhite, bg_color::Green);
    cons.styleLine("Second line", text_color::BrightRed, bg_color::Black);
    cons.styleLine("Third line", text_color::BrightCyan, bg_color::Magenta);

    // ���������� ������ � ������� ���
    cons.setCursorPosition(5, 10);
    cons.styleLine("Positioned text", text_color::BrightGreen, bg_color::Red);



	std::cin.get();
	return 0;
} 