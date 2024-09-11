#include "EngineSettings.h"
#include "Editor.h"

void EngineSettings::Init(Editor& editor)
{
}
void EngineSettings::Update(Editor& editor)
{
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
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
                        editor.Undo();
                    }
                    if (item == "Redo")
                    {
                        editor.Redo();
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

        if (ImGui::BeginMenu("Window"))
        {
            for (const auto& item : winList)
            {
                if (ImGui::MenuItem(item.c_str()))
                {
                    if (item == "FullScreen")
                    {
                        editor.ToggleFullScreen();
                    }
                    if (item == "Close")
                    {
                        editor.CloseEditor();
                    }
                    // Handle file action selection
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
    editor.Save();
    auto entity = editor.entityManager.addEntity("Default");
    entity->addComponent<CTransform>(Vec2(editor.startPosition.x, editor.startPosition.y));
    entity->addComponent<CSize>();
    if (type == "Rectangle")
    {
        std::string name = "Rectangle" + std::to_string(entity->id());
        entity->addComponent<CName>(name);
        entity->addComponent<CShape>();
    }
}