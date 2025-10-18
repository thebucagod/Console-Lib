#include "console.h"
#include <iostream>
#include <thread>
#include <chrono>

void demonstrate_viewport_operations(console& cons) {
    cons.styleLine("=== VIEWPORT OPERATIONS ===", text_color::BrightCyan, bg_color::Black);

    // �������� ������� ����������
    COORD viewportPos = cons.getViewportPosition();
    COORD viewportSize = cons.getViewportSize();
    COORD bufferSize = cons.getBufferSize();

    std::string info = "Current viewport - Position: (" +
        std::to_string(viewportPos.X) + ", " +
        std::to_string(viewportPos.Y) + "), Size: " +
        std::to_string(viewportSize.X) + "x" +
        std::to_string(viewportSize.Y);
    cons.styleLine(info, text_color::White, bg_color::Black);

    cons.moveToNextLine();
}

void demonstrate_buffer_operations(console& cons) {
    cons.styleLine("=== BUFFER OPERATIONS ===", text_color::BrightGreen, bg_color::Black);

    try {
        // ����������� �����
        cons.setBufferSize(120, 50);
        cons.styleLine("? Buffer size set to 120x50", text_color::Green, bg_color::Black);
    } catch (const std::exception& e) {
        cons.styleLine("? Failed to set buffer: " + std::string(e.what()), text_color::Red, bg_color::Black);
    }

    cons.moveToNextLine();
}


void demonstrate_text_styling(console& cons) {
    cons.styleLine("=== TEXT STYLING DEMO ===", text_color::BrightYellow, bg_color::Black);

    // ������������ ������ ������
    cons.styleLine("Standard colors:", text_color::White, bg_color::Black);
    cons.styleLine("Black text", text_color::Black, bg_color::White);
    cons.styleLine("Blue text", text_color::Blue, bg_color::Black);
    cons.styleLine("Green text", text_color::Green, bg_color::Black);
    cons.styleLine("Cyan text", text_color::Cyan, bg_color::Black);
    cons.styleLine("Red text", text_color::Red, bg_color::Black);
    cons.styleLine("Magenta text", text_color::Magenta, bg_color::Black);
    cons.styleLine("Yellow text", text_color::Yellow, bg_color::Black);
    cons.styleLine("White text", text_color::White, bg_color::Black);

    cons.moveToNextLine();

    // ����� �����
    cons.styleLine("Bright colors:", text_color::White, bg_color::Black);
    cons.styleLine("Bright Red", text_color::BrightRed, bg_color::Black);
    cons.styleLine("Bright Green", text_color::BrightGreen, bg_color::Black);
    cons.styleLine("Bright Blue", text_color::BrightBlue, bg_color::Black);
    cons.styleLine("Bright Cyan", text_color::BrightCyan, bg_color::Black);
    cons.styleLine("Bright Magenta", text_color::BrightMagenta, bg_color::Black);
    cons.styleLine("Bright Yellow", text_color::BrightYellow, bg_color::Black);

    cons.moveToNextLine();
}

void demonstrate_background_colors(console& cons) {
    cons.styleLine("=== BACKGROUND COLORS ===", text_color::BrightWhite, bg_color::Black);

    // ������ ���������� ������ � ����
    cons.styleLine("White on Blue", text_color::White, bg_color::Blue);
    cons.styleLine("Black on Green", text_color::Black, bg_color::Green);
    cons.styleLine("Yellow on Red", text_color::Yellow, bg_color::Red);
    cons.styleLine("White on Magenta", text_color::White, bg_color::Magenta);
    cons.styleLine("Black on Cyan", text_color::Black, bg_color::Cyan);
    cons.styleLine("Blue on Yellow", text_color::Blue, bg_color::Yellow);

    cons.moveToNextLine();

    // ����� ����
    cons.styleLine("Bright backgrounds:", text_color::Black, bg_color::BrightWhite);
    cons.styleLine("Bright Red background", text_color::White, bg_color::BrightRed);
    cons.styleLine("Bright Green background", text_color::Black, bg_color::BrightGreen);
    cons.styleLine("Bright Blue background", text_color::White, bg_color::BrightBlue);

    cons.moveToNextLine();
}

void demonstrate_cursor_operations(console& cons) {
    cons.styleLine("=== CURSOR OPERATIONS ===", text_color::BrightMagenta, bg_color::Black);

    // ��������� ������� �������
    COORD originalPos = cons.getCursorPosition();

    // ������� ������ � ������ �������
    cons.setCursorPosition(10, originalPos.Y + 2);
    cons.styleLine("This text starts at position (10, Y+2)", text_color::Cyan, bg_color::Black);

    cons.setCursorPosition(5, originalPos.Y + 4);
    cons.styleLine("This starts at (5, Y+4)", text_color::Green, bg_color::Black);

    cons.setCursorPosition(0, originalPos.Y + 6);
    cons.styleLine("Back to left edge", text_color::Yellow, bg_color::Black);

    cons.moveToNextLine();
}

void demonstrate_advanced_effects(console& cons) {
    cons.styleLine("=== ADVANCED EFFECTS ===", text_color::BrightWhite, bg_color::Blue);

    // �������� ����� - ��������, ��� ������� ����������
    COORD flashPos = cons.getCursorPosition();

    // ������� ������� �������, ����� ��������������� �����
    cons.styleLine(std::string(30, ' '), text_color::Black, bg_color::Black);

    // ������ �������� �����
    std::string flashMessage = ">>> IMPORTANT MESSAGE! <<<";
    for (int i = 0; i < 3; ++i) {
        cons.setCursorPosition(flashPos.X, flashPos.Y);
        cons.styleLine(flashMessage, text_color::BrightRed, bg_color::BrightYellow);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        cons.setCursorPosition(flashPos.X, flashPos.Y);
        cons.styleLine(std::string(flashMessage.length(), ' '), text_color::Black, bg_color::Black);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // ��������� ���������
    cons.setCursorPosition(flashPos.X, flashPos.Y);
    cons.styleLine(flashMessage, text_color::BrightRed, bg_color::BrightYellow);

    // ��������� � ���������� �������
    cons.moveToNextLine();
}

int main() {
    try {
        console cons;

        // ������� ������� (������� ������)
        system("cls");

        std::cout << "Windows Console API Demo\n";
        std::cout << "========================\n\n";

        // ���� ����� ������� ��������� ���������
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // ��������� ������������
        demonstrate_viewport_operations(cons);
        demonstrate_buffer_operations(cons);
        demonstrate_text_styling(cons);
        demonstrate_background_colors(cons);
        demonstrate_cursor_operations(cons);
        demonstrate_advanced_effects(cons);

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}