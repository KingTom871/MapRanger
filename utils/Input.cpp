#include "utils/Input.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {
struct ParsedHotkey {
    UINT modifiers = 0;
    UINT virtualKey = 0;
};

// Trims leading and trailing whitespace from a string.
std::string trim(std::string_view value) {
    const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });

    const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();

    if (begin >= end) {
        return {};
    }

    return std::string(begin, end);
}

// Converts a string to uppercase.
std::string toUpper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

// Splits a hotkey string into tokens by the '+' delimiter, trimming whitespace from each token.
std::vector<std::string> splitHotkey(std::string_view hotkey) {
    std::vector<std::string> tokens;
    std::string current;
    const std::string text(hotkey);
    std::stringstream stream(text);

    while (std::getline(stream, current, '+')) {
        tokens.push_back(trim(current));
    }

    return tokens;
}

// Parses a modifier token into the corresponding MOD_* flag.
std::optional<UINT> parseModifier(const std::string& token) {
    if (token == "CTRL" || token == "CONTROL") {
        return MOD_CONTROL;
    }
    if (token == "ALT") {
        return MOD_ALT;
    }
    if (token == "SHIFT") {
        return MOD_SHIFT;
    }
    if (token == "WIN" || token == "WINDOWS") {
        return MOD_WIN;
    }

    return std::nullopt;
}

// Parses a function key number from a string, returning 0 if the number is invalid or out of range.
int parseFunctionKeyNumber(const std::string& numberText) {
    int number = 0;

    for (const char ch : numberText) {
        number = number * 10 + (ch - '0');
        if (number > 24) {
            return number;
        }
    }

    return number;
}

// Parses a virtual key token into the corresponding UINT value.
// Supporting number keys, function keys, numpad keys, and common special keys.
std::optional<UINT> parseVirtualKey(const std::string& token) {
    if (token.size() == 1) {
        const unsigned char ch = static_cast<unsigned char>(token.front());
        if (std::isalnum(ch) != 0) {
            return static_cast<UINT>(std::toupper(ch));
        }
    }

    if (token.size() >= 2 && token.front() == 'F') {
        const std::string numberText = token.substr(1);
        if (std::all_of(numberText.begin(), numberText.end(), [](unsigned char ch) {
                return std::isdigit(ch) != 0;
            })) {
            const int number = parseFunctionKeyNumber(numberText);
            if (number >= 1 && number <= 24) {
                return static_cast<UINT>(VK_F1 + number - 1);
            }
        }
    }

    if (token.rfind("NUMPAD", 0) == 0 && token.size() == 7 && std::isdigit(static_cast<unsigned char>(token[6])) != 0) {
        return static_cast<UINT>(VK_NUMPAD0 + (token[6] - '0'));
    }

    if (token == "BACKSPACE") return VK_BACK;
    if (token == "TAB") return VK_TAB;
    if (token == "ENTER" || token == "RETURN") return VK_RETURN;
    if (token == "ESC" || token == "ESCAPE") return VK_ESCAPE;
    if (token == "SPACE") return VK_SPACE;
    if (token == "PAGEUP") return VK_PRIOR;
    if (token == "PAGEDOWN") return VK_NEXT;
    if (token == "END") return VK_END;
    if (token == "HOME") return VK_HOME;
    if (token == "LEFT") return VK_LEFT;
    if (token == "UP") return VK_UP;
    if (token == "RIGHT") return VK_RIGHT;
    if (token == "DOWN") return VK_DOWN;
    if (token == "INSERT" || token == "INS") return VK_INSERT;
    if (token == "DELETE" || token == "DEL") return VK_DELETE;
    if (token == "NUMLOCK") return VK_NUMLOCK;
    if (token == "SCROLLLOCK") return VK_SCROLL;
    if (token == "PRINTSCREEN") return VK_SNAPSHOT;
    if (token == "PAUSE") return VK_PAUSE;
    if (token == "CAPSLOCK") return VK_CAPITAL;
    if (token == "OEM_MINUS") return VK_OEM_MINUS;
    if (token == "OEM_PLUS") return VK_OEM_PLUS;
    if (token == "OEM_COMMA") return VK_OEM_COMMA;
    if (token == "OEM_PERIOD") return VK_OEM_PERIOD;

    return std::nullopt;
}

// Parses a hotkey string into a ParsedHotkey structure, validating the format and supported keys/modifiers.
std::optional<ParsedHotkey> parseHotkey(std::string_view hotkey, std::string_view label) {
    const std::vector<std::string> tokens = splitHotkey(hotkey);

    if (tokens.empty()) {
        std::cerr << "[Input] Invalid " << label << " hotkey: empty value\n";
        return std::nullopt;
    }

    ParsedHotkey parsed;
    bool hasKey = false;

    for (const std::string& rawToken : tokens) {
        const std::string token = toUpper(rawToken);

        if (token.empty()) {
            std::cerr << "[Input] Invalid " << label << " hotkey: empty key segment\n";
            return std::nullopt;
        }

        if (const auto modifier = parseModifier(token)) {
            if ((parsed.modifiers & *modifier) != 0) {
                std::cerr << "[Input] Invalid " << label << " hotkey: duplicate modifier " << rawToken << "\n";
                return std::nullopt;
            }

            parsed.modifiers |= *modifier;
            continue;
        }

        if (hasKey) {
            std::cerr << "[Input] Invalid " << label << " hotkey: only one non-modifier key is allowed\n";
            return std::nullopt;
        }

        const auto virtualKey = parseVirtualKey(token);
        if (!virtualKey) {
            std::cerr << "[Input] Invalid " << label << " hotkey: unsupported key " << rawToken << "\n";
            return std::nullopt;
        }

        parsed.virtualKey = *virtualKey;
        hasKey = true;
    }

    if (!hasKey) {
        std::cerr << "[Input] Invalid " << label << " hotkey: missing non-modifier key\n";
        return std::nullopt;
    }

    parsed.modifiers |= MOD_NOREPEAT;
    return parsed;
}

// Registers a global hotkey based on the provided configuration, logging success or failure.
bool registerConfiguredHotkey(HWND hwnd, int id, std::string_view label, std::string_view text) {
    const auto parsed = parseHotkey(text, label);
    if (!parsed) {
        return false;
    }

    if (RegisterHotKey(hwnd, id, parsed->modifiers, parsed->virtualKey) == FALSE) {
        std::cerr << "[Input] Failed to register " << label << " hotkey: " << text
                  << " (error " << GetLastError() << ")\n";
        return false;
    }

    std::cout << "[Input] Registered " << label << " hotkey: " << text << "\n";
    return true;
}
}

Input::Input(const RuntimeConfig& config)
    : config(config) {
}

Input::~Input() {
    unregisterHotkeys();
}

// Registers configured global hotkeys for the supplied window.
bool Input::initialize(HWND targetHwnd) {
    unregisterHotkeys();
    hwnd = targetHwnd;

    miniMapHotkeyRegistered = registerConfiguredHotkey(
        hwnd,
        MiniMapHotkeyId,
        "minimapOverlayHotkey",
        config.minimapOverlayHotkey
    );

    if (!miniMapHotkeyRegistered) {
        unregisterHotkeys();
        return false;
    }

    return true;
}

void Input::processMessage(const MSG& msg) {
    if (msg.message != WM_HOTKEY) {
        return;
    }

    switch (static_cast<int>(msg.wParam)) {
    case MiniMapHotkeyId:
        minimapEnabled = !minimapEnabled;
        std::cout << (minimapEnabled ? "[MiniMapOverlay] Visible\n" : "[MiniMapOverlay] Hidden\n");
        break;

    default:
        break;
    }
}

bool Input::isMiniMapEnabled() const {
    return minimapEnabled;
}

void Input::unregisterHotkeys() {
    if (hwnd == nullptr && !miniMapHotkeyRegistered) {
        return;
    }

    if (miniMapHotkeyRegistered) {
        UnregisterHotKey(hwnd, MiniMapHotkeyId);
        miniMapHotkeyRegistered = false;
    }

    hwnd = nullptr;
}
