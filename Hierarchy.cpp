#include "Hierarchy.h"
#include "Editor.h"

void Hierarchy::Init(Editor& editor)
{
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
        if (ImGui::Selectable(entity->getComponent<CName>().name.c_str(), editor.selectedEntity == entity, ImGuiSelectableFlags_AllowDoubleClick))
        {
            editor.selectedEntity = entity;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                name = entity->getComponent<CName>().name;
                editingEntity = entity;
                std::cout << editor.selectedEntity->id() << std::endl;
            }
        }
        if (editingEntity == entity)
        {
            ImGui::InputText("Rename", &name[0], name.size() + 1);
            if (ImGui::Button("Save"))
            {
                entity->getComponent<CName>().name = name;
                name = "";
                editingEntity = nullptr;
                editor.command.Save(editor.entityManager);
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                name = "";
                editingEntity = nullptr;
            }
        }
        ImGui::PopID();
    }
}