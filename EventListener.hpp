#include <string>
#include <unordered_map>
#include "imgui.h"

class EventListener
{
	std::unordered_map<std::string, ImGuiKey> keyMap = {
	{"A", ImGuiKey_A}, {"B", ImGuiKey_B}, {"C", ImGuiKey_C}, {"D", ImGuiKey_D},
	{"E", ImGuiKey_E}, {"F", ImGuiKey_F}, {"G", ImGuiKey_G}, {"H", ImGuiKey_H},
	{"I", ImGuiKey_I}, {"J", ImGuiKey_J}, {"K", ImGuiKey_K}, {"L", ImGuiKey_L},
	{"M", ImGuiKey_M}, {"N", ImGuiKey_N}, {"O", ImGuiKey_O}, {"P", ImGuiKey_P},
	{"Q", ImGuiKey_Q}, {"R", ImGuiKey_R}, {"S", ImGuiKey_S}, {"T", ImGuiKey_T},
	{"U", ImGuiKey_U}, {"V", ImGuiKey_V}, {"W", ImGuiKey_W}, {"X", ImGuiKey_X},
	{"Y", ImGuiKey_Y}, {"Z", ImGuiKey_Z}, {"Escape", ImGuiKey_Escape},
	{"Enter", ImGuiKey_Enter}, {"Tab", ImGuiKey_Tab}, {"Space", ImGuiKey_Space},
	{"LeftShift", ImGuiKey_LeftShift}, {"RightShift", ImGuiKey_RightShift},
	{"LeftCtrl", ImGuiKey_LeftCtrl}, {"RightCtrl", ImGuiKey_RightCtrl},
	{"LeftAlt", ImGuiKey_LeftAlt}, {"RightAlt", ImGuiKey_RightAlt},
	{"LeftSuper", ImGuiKey_LeftSuper}, {"RightSuper", ImGuiKey_RightSuper},
	{"Menu", ImGuiKey_Menu}, {"Up", ImGuiKey_UpArrow}, {"Down", ImGuiKey_DownArrow},
	{"Left", ImGuiKey_LeftArrow}, {"Right", ImGuiKey_RightArrow},
	{"Backspace", ImGuiKey_Backspace}, {"Delete", ImGuiKey_Delete},
	{"Insert", ImGuiKey_Insert}, {"Home", ImGuiKey_Home}, {"End", ImGuiKey_End},
	{"PageUp", ImGuiKey_PageUp}, {"PageDown", ImGuiKey_PageDown}, {"F1", ImGuiKey_F1},
	{"F2", ImGuiKey_F2}, {"F3", ImGuiKey_F3}, {"F4", ImGuiKey_F4},
	{"F5", ImGuiKey_F5}, {"F6", ImGuiKey_F6}, {"F7", ImGuiKey_F7},
	{"F8", ImGuiKey_F8}, {"F9", ImGuiKey_F9}, {"F10", ImGuiKey_F10},
	{"F11", ImGuiKey_F11}, {"F12", ImGuiKey_F12}, {"Numpad0", ImGuiKey_Keypad0},
	{"Numpad1", ImGuiKey_Keypad1}, {"Numpad2", ImGuiKey_Keypad2}, {"Numpad3", ImGuiKey_Keypad3},
	{"Numpad4", ImGuiKey_Keypad4}, {"Numpad5", ImGuiKey_Keypad5}, {"Numpad6", ImGuiKey_Keypad6},
	{"Numpad7", ImGuiKey_Keypad7}, {"Numpad8", ImGuiKey_Keypad8}, {"Numpad9", ImGuiKey_Keypad9},
	{"NumpadEnter", ImGuiKey_KeypadEnter}, {"Add", ImGuiKey_KeypadAdd},
	{"Subtract", ImGuiKey_KeypadSubtract}, {"Multiply", ImGuiKey_KeypadMultiply},
	{"Divide", ImGuiKey_KeypadDivide}
	};
public:
	const bool IsKeyDown(const std::string& key)
	{
		return ImGui::IsKeyDown(keyMap[key]);
	}

	const bool IsKeyPressed(const std::string& key)
	{
		return ImGui::IsKeyPressed(keyMap[key]);
	}

	const bool IsKeyReleased(const std::string& key)
	{
		return ImGui::IsKeyReleased(keyMap[key]);
	}
	static void Lua(sol::state& lua)
	{
		lua.new_usertype<EventListener>("EventListener",
			"IsKeyDown", &EventListener::IsKeyDown,
			"IsKeyPressed", &EventListener::IsKeyPressed,
			"IsKeyReleased", &EventListener::IsKeyReleased
		);
	}
};