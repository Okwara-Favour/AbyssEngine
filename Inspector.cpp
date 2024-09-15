#include "Inspector.h"
#include "Editor.h"

void Inspector::Init(Editor& editor)
{

}
void Inspector::Update(Editor& editor)
{
	ImGui::Begin("Inspector");
    ImGui::PushItemWidth(100);
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
    ImGui::PushItemWidth(25);
    if (ImGui::BeginCombo("Add Component", " "))
    {
        for (const auto& component : CList)
        {
            if (ImGui::Selectable(component.c_str()))
            {
                if (component == "Transform") 
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CTransform>(Vec2(editor.startPosition.x, editor.startPosition.y));
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

    ImGui::SameLine();
    if (ImGui::BeginCombo("Remove Component", " "))
    {
        for (const auto& component : CList)
        {
            if (ImGui::Selectable(component.c_str()))
            {
                if (component == "Transform")
                {
                    editor.Save();
                    editor.selectedEntity->removeComponent<CTransform>();
                }
                if (component == "Renderer")
                {
                    editor.Save();
                    editor.selectedEntity->removeComponent<CShape>();
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
}

void Inspector::displayComponents(Editor& editor)
{
    if (editor.selectedEntity->hasComponent<CTransform>())
    {
        auto& trans = editor.selectedEntity->getComponent<CTransform>();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& trans = editor.selectedEntity->getComponent<CTransform>();
            ImGui::Text("Position");
            ImGui::InputFloat("X", &trans.pos.x);
            ImGui::SameLine();
            ImGui::InputFloat("Y", &trans.pos.y);
            ImGui::Separator(); // Adds a horizontal line separator
            ImGui::Text("Scale");
            ImGui::InputFloat("X", &trans.scale.x);
            ImGui::SameLine();
            ImGui::InputFloat("Y", &trans.scale.y);
            ImGui::Separator();
            ImGui::Text("Rotation");
            ImGui::InputFloat("Angle", &trans.angle);
        }
    }

    if (editor.selectedEntity->hasComponent<CShape>())
    {
        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Nothing to see here yet");
        }
    }
}