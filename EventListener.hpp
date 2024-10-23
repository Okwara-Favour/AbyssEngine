#include <string>
#include <unordered_map>
#include "imgui.h"
#include <SFML/Window/Keyboard.hpp>

class EventListener
{
	std::unordered_map<std::string, ImGuiKey> imguiKeyMap = {
		{"None", ImGuiKey_None}, {"Tab", ImGuiKey_Tab}, {"LeftArrow", ImGuiKey_LeftArrow}, {"RightArrow", ImGuiKey_RightArrow},
		{"UpArrow", ImGuiKey_UpArrow}, {"DownArrow", ImGuiKey_DownArrow}, {"PageUp", ImGuiKey_PageUp}, {"PageDown", ImGuiKey_PageDown},
		{"Home", ImGuiKey_Home}, {"End", ImGuiKey_End}, {"Insert", ImGuiKey_Insert}, {"Delete", ImGuiKey_Delete},
		{"Backspace", ImGuiKey_Backspace}, {"Space", ImGuiKey_Space}, {"Enter", ImGuiKey_Enter}, {"Escape", ImGuiKey_Escape},
		{"LeftCtrl", ImGuiKey_LeftCtrl}, {"LeftShift", ImGuiKey_LeftShift}, {"LeftAlt", ImGuiKey_LeftAlt}, {"LeftSuper", ImGuiKey_LeftSuper},
		{"RightCtrl", ImGuiKey_RightCtrl}, {"RightShift", ImGuiKey_RightShift}, {"RightAlt", ImGuiKey_RightAlt}, {"RightSuper", ImGuiKey_RightSuper},
		{"Menu", ImGuiKey_Menu}, {"0", ImGuiKey_0}, {"1", ImGuiKey_1}, {"2", ImGuiKey_2}, {"3", ImGuiKey_3}, {"4", ImGuiKey_4}, {"5", ImGuiKey_5},
		{"6", ImGuiKey_6}, {"7", ImGuiKey_7}, {"8", ImGuiKey_8}, {"9", ImGuiKey_9}, {"A", ImGuiKey_A}, {"B", ImGuiKey_B}, {"C", ImGuiKey_C},
		{"D", ImGuiKey_D}, {"E", ImGuiKey_E}, {"F", ImGuiKey_F}, {"G", ImGuiKey_G}, {"H", ImGuiKey_H}, {"I", ImGuiKey_I}, {"J", ImGuiKey_J},
		{"K", ImGuiKey_K}, {"L", ImGuiKey_L}, {"M", ImGuiKey_M}, {"N", ImGuiKey_N}, {"O", ImGuiKey_O}, {"P", ImGuiKey_P}, {"Q", ImGuiKey_Q},
		{"R", ImGuiKey_R}, {"S", ImGuiKey_S}, {"T", ImGuiKey_T}, {"U", ImGuiKey_U}, {"V", ImGuiKey_V}, {"W", ImGuiKey_W}, {"X", ImGuiKey_X},
		{"Y", ImGuiKey_Y}, {"Z", ImGuiKey_Z}, {"F1", ImGuiKey_F1}, {"F2", ImGuiKey_F2}, {"F3", ImGuiKey_F3}, {"F4", ImGuiKey_F4},
		{"F5", ImGuiKey_F5}, {"F6", ImGuiKey_F6}, {"F7", ImGuiKey_F7}, {"F8", ImGuiKey_F8}, {"F9", ImGuiKey_F9}, {"F10", ImGuiKey_F10},
		{"F11", ImGuiKey_F11}, {"F12", ImGuiKey_F12}, {"F13", ImGuiKey_F13}, {"F14", ImGuiKey_F14}, {"F15", ImGuiKey_F15}, {"F16", ImGuiKey_F16},
		{"F17", ImGuiKey_F17}, {"F18", ImGuiKey_F18}, {"F19", ImGuiKey_F19}, {"F20", ImGuiKey_F20}, {"F21", ImGuiKey_F21}, {"F22", ImGuiKey_F22},
		{"F23", ImGuiKey_F23}, {"F24", ImGuiKey_F24}, {"Apostrophe", ImGuiKey_Apostrophe}, {"Comma", ImGuiKey_Comma}, {"Minus", ImGuiKey_Minus},
		{"Period", ImGuiKey_Period}, {"Slash", ImGuiKey_Slash}, {"Semicolon", ImGuiKey_Semicolon}, {"Equal", ImGuiKey_Equal},
		{"LeftBracket", ImGuiKey_LeftBracket}, {"Backslash", ImGuiKey_Backslash}, {"RightBracket", ImGuiKey_RightBracket},
		{"GraveAccent", ImGuiKey_GraveAccent}, {"CapsLock", ImGuiKey_CapsLock}, {"ScrollLock", ImGuiKey_ScrollLock}, {"NumLock", ImGuiKey_NumLock},
		{"PrintScreen", ImGuiKey_PrintScreen}, {"Pause", ImGuiKey_Pause}, {"Keypad0", ImGuiKey_Keypad0}, {"Keypad1", ImGuiKey_Keypad1},
		{"Keypad2", ImGuiKey_Keypad2}, {"Keypad3", ImGuiKey_Keypad3}, {"Keypad4", ImGuiKey_Keypad4}, {"Keypad5", ImGuiKey_Keypad5},
		{"Keypad6", ImGuiKey_Keypad6}, {"Keypad7", ImGuiKey_Keypad7}, {"Keypad8", ImGuiKey_Keypad8}, {"Keypad9", ImGuiKey_Keypad9},
		{"KeypadDecimal", ImGuiKey_KeypadDecimal}, {"KeypadDivide", ImGuiKey_KeypadDivide}, {"KeypadMultiply", ImGuiKey_KeypadMultiply},
		{"KeypadSubtract", ImGuiKey_KeypadSubtract}, {"KeypadAdd", ImGuiKey_KeypadAdd}, {"KeypadEnter", ImGuiKey_KeypadEnter},
		{"KeypadEqual", ImGuiKey_KeypadEqual}
	};
	std::unordered_map<std::string, sf::Keyboard::Key> keyMap = {
		{"A", sf::Keyboard::A}, {"B", sf::Keyboard::B}, {"C", sf::Keyboard::C}, {"D", sf::Keyboard::D},
		{"E", sf::Keyboard::E}, {"F", sf::Keyboard::F}, {"G", sf::Keyboard::G}, {"H", sf::Keyboard::H},
		{"I", sf::Keyboard::I}, {"J", sf::Keyboard::J}, {"K", sf::Keyboard::K}, {"L", sf::Keyboard::L},
		{"M", sf::Keyboard::M}, {"N", sf::Keyboard::N}, {"O", sf::Keyboard::O}, {"P", sf::Keyboard::P},
		{"Q", sf::Keyboard::Q}, {"R", sf::Keyboard::R}, {"S", sf::Keyboard::S}, {"T", sf::Keyboard::T},
		{"U", sf::Keyboard::U}, {"V", sf::Keyboard::V}, {"W", sf::Keyboard::W}, {"X", sf::Keyboard::X},
		{"Y", sf::Keyboard::Y}, {"Z", sf::Keyboard::Z}, {"Escape", sf::Keyboard::Escape},
		{"Enter", sf::Keyboard::Enter}, {"Tab", sf::Keyboard::Tab}, {"Space", sf::Keyboard::Space},
		{"LeftShift", sf::Keyboard::LShift}, {"RightShift", sf::Keyboard::RShift},
		{"LeftCtrl", sf::Keyboard::LControl}, {"RightCtrl", sf::Keyboard::RControl},
		{"LeftAlt", sf::Keyboard::LAlt}, {"RightAlt", sf::Keyboard::RAlt},
		{"LeftSuper", sf::Keyboard::LSystem}, {"RightSuper", sf::Keyboard::RSystem},
		{"Menu", sf::Keyboard::Menu}, {"Up", sf::Keyboard::Up}, {"Down", sf::Keyboard::Down},
		{"Left", sf::Keyboard::Left}, {"Right", sf::Keyboard::Right},
		{"Backspace", sf::Keyboard::Backspace}, {"Delete", sf::Keyboard::Delete},
		{"Insert", sf::Keyboard::Insert}, {"Home", sf::Keyboard::Home}, {"End", sf::Keyboard::End},
		{"PageUp", sf::Keyboard::PageUp}, {"PageDown", sf::Keyboard::PageDown}, {"F1", sf::Keyboard::F1},
		{"F2", sf::Keyboard::F2}, {"F3", sf::Keyboard::F3}, {"F4", sf::Keyboard::F4},
		{"F5", sf::Keyboard::F5}, {"F6", sf::Keyboard::F6}, {"F7", sf::Keyboard::F7},
		{"F8", sf::Keyboard::F8}, {"F9", sf::Keyboard::F9}, {"F10", sf::Keyboard::F10},
		{"F11", sf::Keyboard::F11}, {"F12", sf::Keyboard::F12}, {"Numpad0", sf::Keyboard::Numpad0},
		{"Numpad1", sf::Keyboard::Numpad1}, {"Numpad2", sf::Keyboard::Numpad2}, {"Numpad3", sf::Keyboard::Numpad3},
		{"Numpad4", sf::Keyboard::Numpad4}, {"Numpad5", sf::Keyboard::Numpad5}, {"Numpad6", sf::Keyboard::Numpad6},
		{"Numpad7", sf::Keyboard::Numpad7}, {"Numpad8", sf::Keyboard::Numpad8}, {"Numpad9", sf::Keyboard::Numpad9},
		{"NumpadEnter", sf::Keyboard::Enter}, {"Add", sf::Keyboard::Add},
		{"Subtract", sf::Keyboard::Subtract}, {"Multiply", sf::Keyboard::Multiply},
		{"Divide", sf::Keyboard::Divide}
	};
public:
	const bool UIIsKeyPressed(const std::string& key)
	{
		if(imguiKeyMap.find(key) != imguiKeyMap.end())	return ImGui::IsKeyPressed(imguiKeyMap[key]);
		return false;
	}

	const bool IsKeyPressed(const std::string& key)
	{
		if(keyMap.find(key) != keyMap.end()) return sf::Keyboard::isKeyPressed(keyMap[key]);
		return false;
	}
	// If you need key release tracking, track state changes manually
	const bool IsKeyReleased(const std::string& key)
	{
		if (keyMap.find(key) != keyMap.end()) return !sf::Keyboard::isKeyPressed(keyMap[key]); // Simplistic version
		return false;
	}
	static void Lua(sol::state& lua)
	{
		lua.new_usertype<EventListener>("EventListener",
			"IsKeyPressed", &EventListener::IsKeyPressed,
			"UIIsKeyPressed", &EventListener::UIIsKeyPressed,
			"IsKeyReleased", &EventListener::IsKeyReleased
		);
	}
};