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
        displayComponents(editor);
        handleComponents(editor);
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
                if (editor.selectedEntity->tag() != tag) {
                    editor.Save();
                    editor.entityManager.changeTag(editor.selectedEntity, tag);
                }
            }
        }
        ImGui::EndCombo();
    }
}

void Inspector::handleComponents(Editor& editor)
{
    if (ImGui::BeginCombo("Add Component", " "))
    {
        for (const auto& component : CList)
        {
            if (ImGui::Selectable(component.c_str()))
            {
                if (component == "Transform") 
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CTransform>();
                }
                if (component == "Renderer")
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CShape>();
                }
            }
        }
        ImGui::EndCombo();
    }
}

void Inspector::displayComponents(Editor& editor)
{
    if (editor.selectedEntity->hasComponent<CTransform>())
    {
        auto& trans = editor.selectedEntity->getComponent<CTransform>();
        ImGui::Text("Transform");
        ImGui::Text("px: %f", trans.pos.x);
        ImGui::SameLine();
        ImGui::Text("py: %f", trans.pos.y);
        ImGui::Text("sx: %f", trans.scale.x);
        ImGui::SameLine();
        ImGui::Text("sy: %f", trans.scale.y);
        ImGui::Text("a: %f", trans.angle);
    }

    if (editor.selectedEntity->hasComponent<CShape>())
    {
        ImGui::Text("Renderer");
        ImGui::Text("Nothing to see here yet");
    }
}