#include "Inspector.h"

void Inspector::Init(Editor& editor)
{

}
void Inspector::Update(Editor& editor)
{
	ImGui::Begin("Inspector");
	ImGui::Text("Content of Tab Inspector");
	ImGui::End();
}
