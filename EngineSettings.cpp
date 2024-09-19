#include "EngineSettings.h"
#include "Editor.h"
#include "NFD/include/nfd.h"
#include <sstream>

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
                    if (item == "Import")
                    {
                        ImportFiles(editor);
                    }
                    if (item == "Save")
                    {
                        editor.SaveScene();
                    }
                    if (item == "Load")
                    {
                        editor.load = true;
                    }
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
                        editor.Save();
                        editor.entityManager.destroyEntity(editor.selectedEntity);
                        editor.selectedEntity = nullptr;
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
        entity->addComponent<CRectangleShape>();
    }
}

void EngineSettings::ImportFiles(Editor& editor)
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath); // Open a dialog to choose a file
    std::stringstream iss;
    if (result == NFD_OKAY) // If the user selected a file
    {
        iss << "User selected file: " << outPath;
        editor.HandleError(iss.str());
        iss.str("");
        // You can use fs to copy the file to the desired directory
        fs::path sourceFile(outPath);
        fs::path targetDirectory = editor.currentDirectory; // Your current directory

        try
        {
            fs::copy(sourceFile, targetDirectory / sourceFile.filename(),
                fs::copy_options::overwrite_existing);
            editor.HandleError("File imported successfully.");
        }
        catch (const fs::filesystem_error& e)
        {
            iss << "Error copying file: " << e.what();
            editor.HandleError(iss.str());
            iss.str("");
        }
    }
    else if (result == NFD_CANCEL) // If the user canceled the file selection
    {
        editor.HandleError("User canceled the operation.");
    }
    else // If there was an error
    {
        iss << "Error: " << NFD_GetError();
        editor.HandleError(iss.str());
        iss.str("");
    }
    // Free the memory allocated by nfd
    if (outPath)
    {
        free(outPath);
    }
}