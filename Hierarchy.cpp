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

    if (ImGui::BeginTabBar("EntityTabs"))
    {
        // Tab for displaying entities
        if (ImGui::BeginTabItem("Entities"))
        {
            editor.selectedPrefab = nullptr;
            DisplayEntities(editor);
            ImGui::EndTabItem();
        }

        // Tab for displaying prefabs
        if (ImGui::BeginTabItem("Prefabs"))
        {
            editor.selectedEntity = nullptr;
            DisplayPrefabs(editor); // Assuming you have a DisplayPrefabs function
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (clock.getElapsedTime() > sf::seconds(duration)) clickCount = 0;
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (!ImGui::IsAnyItemHovered())
        {
            // Reset the current selection to null if no entity is hovered and the mouse is clicked
            editor.selectedEntity = nullptr;
            editor.selectedPrefab = nullptr;
        }
    }
	ImGui::End();
}

void Hierarchy::DisplayEntities(Editor& editor)
{
    const auto& entities = editor.entityManager.getEntities();

    for (const auto& entity : entities)
    {
        if (entity->hasComponent<CParent>()) continue;
        ImGui::PushID(entity->id());

        if (entity->hasComponent<CChildren>())
        {
            auto& children = entity->getComponent<CChildren>();
            if (ImGui::ArrowButton("##arrow", (children._editor_use_open) ? ImGuiDir_Down : ImGuiDir_Right))
            {
                children._editor_use_open = !children._editor_use_open;
            }
            ImGui::SameLine();
        }

        if (editor.parentEntity && editor.parentEntity->id() == entity->id()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        }

        if (ImGui::Selectable(entity->getComponent<CName>().name.c_str(), editor.selectedEntity && editor.selectedEntity->id() == entity->id(), ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (editor.selectedEntity && editor.selectedEntity->id() != entity->id()) editor.Save();
            editor.selectedEntity = entity;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                auto& name = entity->getComponent<CName>().name;
                std::fill(nameBuffer, nameBuffer + sizeof(nameBuffer), '\0');
                std::copy(name.begin(), name.end(), nameBuffer);
                editingEntity = entity;
            }
        }

        if (editor.parentEntity && editor.parentEntity->id() == entity->id()) {
            ImGui::PopStyleColor();
        }

        editEntityName(editor, entity);
        ParentChildDropdown(editor, entity);
        
        ImGui::PopID();
    }
}

void Hierarchy::DisplayPrefabs(Editor& editor)
{
    const auto& entities = editor.prefabManager.getEntities();

    for (const auto& entity : entities)
    {
        if (entity->hasComponent<CParent>()) continue;
        ImGui::PushID(entity->id());

        if (entity->hasComponent<CChildren>())
        {
            auto& children = entity->getComponent<CChildren>();
            if (ImGui::ArrowButton("##arrow", (children._editor_use_open) ? ImGuiDir_Down : ImGuiDir_Right))
            {
                children._editor_use_open = !children._editor_use_open;
            }
            ImGui::SameLine();
        }

        if (ImGui::Selectable(entity->getComponent<CName>().name.c_str(), editor.selectedPrefab && editor.selectedPrefab->id() == entity->id(), ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (editor.selectedPrefab && editor.selectedPrefab->id() != entity->id()) editor.Save();
            editor.selectedPrefab = entity;
            editor.selectedEntity = nullptr;
            //std::cout << editor.selectedPrefab->id() << std::endl;
            /*if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                auto& name = entity->getComponent<CName>().name;
                std::fill(nameBuffer, nameBuffer + sizeof(nameBuffer), '\0');
                std::copy(name.begin(), name.end(), nameBuffer);
                editingEntity = entity;
            }*/
        }
        ImGui::PopID();
    }
}

void Hierarchy::ParentChildDropdown(Editor& editor, std::shared_ptr<Entity> entity)
{
    if (entity->hasComponent<CChildren>())
    {
        auto& children = entity->getComponent<CChildren>();
        if (children._editor_use_open)
        {
            for (auto& c : entity->getComponent<CChildren>().children)
            {
                auto& childEntity = editor.entityManager.getEntity(c.first);
                ImGui::PushID(childEntity->id());
                if (childEntity->hasComponent<CChildren>())
                {
                    auto& grandChildren = childEntity->getComponent<CChildren>();
                    if (ImGui::ArrowButton("##arrow", (grandChildren._editor_use_open) ? ImGuiDir_Down : ImGuiDir_Right))
                    {
                        grandChildren._editor_use_open = !grandChildren._editor_use_open;
                    }
                    ImGui::SameLine();
                }

                if (editor.parentEntity && editor.parentEntity->id() == childEntity->id()) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                }

                if (ImGui::Selectable(childEntity->getComponent<CName>().name.c_str(), editor.selectedEntity && editor.selectedEntity->id() == childEntity->id()))
                {
                    clickCount++;
                    if (editor.selectedEntity && editor.selectedEntity->id() != childEntity->id()) editor.Save();
                    editor.selectedEntity = childEntity;
                    bool doubleClick = clickCount == 2 && clock.getElapsedTime() <= sf::seconds(duration);
                    if (ImGui::IsItemHovered() && doubleClick)
                    {
                        auto& name = childEntity->getComponent<CName>().name;
                        std::fill(nameBuffer, nameBuffer + sizeof(nameBuffer), '\0');
                        std::copy(name.begin(), name.end(), nameBuffer);
                        editingEntity = childEntity;
                    }
                    clock.restart();
                    if (doubleClick) clickCount = 0;
                }

                if (editor.parentEntity && editor.parentEntity->id() == childEntity->id()) {
                    ImGui::PopStyleColor();
                }

                editEntityName(editor, childEntity);
                ParentChildDropdown(editor, childEntity);
                ImGui::PopID();
            }
        }
    }
}


void Hierarchy::editEntityName(Editor& editor, std::shared_ptr<Entity> entity)
{
    if (editingEntity && editingEntity->id() == entity->id())
    {
        ImGui::InputText("Rename", &nameBuffer[0], 50);

        if (editor.isMouseInTab())
        {
            for (auto& key : keyAction)
            {
                if (ImGui::IsKeyPressed(key.first))
                {
                    if (key.second == "ENTER")
                    {
                        std::string nameData = nameBuffer;
                        entity->getComponent<CName>().name = nameData;
                        if (nameData != entity->getComponent<CName>().name) editor.Save();
                        editingEntity = nullptr;
                    }
                    if (key.second == "ESCAPE")
                    {
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
}