#include "Inspector.h"
#include "Editor.h"

void Inspector::Init(Editor& editor)
{

}
void Inspector::Update(Editor& editor)
{
	ImGui::Begin("Inspector");
    ImGui::PushItemWidth(100);
	ImGui::Text("Content of Tab Inspector");
	if (editor.selectedEntity)
	{
        handleTags(editor);
	}
    ImGui::PopItemWidth();
	ImGui::End();
}

void Inspector::handleTags(Editor& editor)
{
    if (ImGui::BeginCombo("Tag", editor.selectedEntity->tag().c_str()))
    {
        for (const auto& tag : Tags)
        {
            if (ImGui::Selectable(tag.c_str()))
            {
                editor.entityManager.changeTag(editor.selectedEntity, tag);
            }
        }
        ImGui::EndCombo();
    }
}
