#include "Display.h"

void Display::Init(Editor& editor)
{

}
void Display::Update(Editor& editor)
{
	ImGui::Begin("Display");
	ImGui::Text("Content of Tab Display");
	ImGui::End();
}