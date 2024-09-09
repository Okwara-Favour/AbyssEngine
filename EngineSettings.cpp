#include "EngineSettings.h"
#include "Editor.h"

void EngineSettings::Init(Editor& editor)
{
}
void EngineSettings::Update(Editor& editor)
{
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            for (const auto& item : fileList)
            {
                if (ImGui::MenuItem(item.c_str()))
                {
                    // Handle file action selection
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            for (const auto& item : editList)
            {
                if (ImGui::MenuItem(item.c_str()))
                {
                    if (item == "Undo")
                    {
                        editor.command.Undo(editor.entityManager);
                    }
                    if (item == "Redo")
                    {
                        editor.command.Redo(editor.entityManager);
                    }
                    // Handle edit action selection
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject"))
        {
            for (const auto& item : gameObjectList)
            {
                if (item != "Add" && ImGui::MenuItem(item.c_str()))
                {
                    if (item == "Remove")
                    {
                        editor.entityManager.destroyEntity(editor.selectedEntity);
                    }
                }
                if (item == "Add" && ImGui::BeginMenu("Add"))
                {
                    for (const auto& addItem : addList)
                    {
                        if (ImGui::MenuItem(addItem.c_str()))
                        {
                            createEntity(editor, addItem);
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void EngineSettings::createEntity(Editor& editor, const std::string& type)
{
    editor.command.Save(editor.entityManager);
    if (type == "Rectangle")
    {
        auto rect = editor.entityManager.addEntity("Default");
        std::string name = "Rectangle" + std::to_string(rect->id());
        rect->addComponent<CName>(name);
    }
}