#include "EngineSettings.h"

void EngineSettings::Init(Editor& editor)
{

}
void EngineSettings::Update(Editor& editor)
{
	ImGui::Begin("Settings");
	ImGui::Text("Content of Tab Settings");
	ImGui::End();
}
