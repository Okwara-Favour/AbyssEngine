#include "Hierarchy.h"
#include "Editor.h"

void Hierarchy::Init(Editor& editor)
{
    keyAction[ImGuiKey_Enter] = "ENTER";
    keyAction[ImGuiKey_Escape] = "ESCAPE";
}
void Hierarchy::Update(Editor& editor)
{
	ImGui::Begin("Hierarchy");
	ImGui::Text("Content of Tab Hierarchy");
    DisplayEntities(editor);

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (!ImGui::IsAnyItemHovered())
        {
            // Reset the current selection to null if no entity is hovered and the mouse is clicked
            editor.selectedEntity = nullptr;
        }
    }
	ImGui::End();
}

void Hierarchy::DisplayEntities(Editor& editor)
{
    const auto& entities = editor.entityManager.getEntities();

    for (const auto& entity : entities)
    {
        ImGui::PushID(entity->id());  // Assuming entity has a GetID() method
        if (ImGui::Selectable(entity->getComponent<CName>().name.c_str(), editor.selectedEntity && editor.selectedEntity->id() == entity->id(), ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (editor.selectedEntity && editor.selectedEntity->id() != entity->id()) editor.Save();
            editor.selectedEntity = entity;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                name = entity->getComponent<CName>().name;
                editingEntity = entity;
            }
        }
        if (editingEntity && editingEntity->id() == entity->id())
        {
            ImGui::InputText("Rename", &name[0], name.size() + 1);

            if (editor.isMouseInTab())
            {
                for (auto& key : keyAction)
                {
                    if (ImGui::IsKeyPressed(key.first))
                    {
                        if (key.second == "ENTER")
                        {
                            entity->getComponent<CName>().name = name;
                            if (name != entity->getComponent<CName>().name) editor.Save();
                            name = "";
                            editingEntity = nullptr;
                        }
                        if (key.second == "ESCAPE")
                        {
                            name = "";
                            editingEntity = nullptr;
                        }
                    }
                }
            }
            if (!editor.selectedEntity || (editor.selectedEntity && editingEntity && editingEntity->id() != editor.selectedEntity->id()))
            {
                editingEntity = nullptr;
            }
        }
        ImGui::PopID();
    }
}