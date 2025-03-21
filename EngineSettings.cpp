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
        if (!editor.gameMode)
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
                            if (editor.saveFile == "") 
                            {
                                MakeFile(editor);
                            }
                            if (editor.saveFile != "")
                            {
                                editor.SaveScene();
                                editor.SavePrefabData();
                            }
                            
                        }
                        if (item == "Load")
                        {
                            LoadFile(editor);
                            if(editor.saveFile != "") editor.load = true;
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
                            editor.parentEntity = nullptr;
                            editor.Undo();
                        }
                        if (item == "Redo")
                        {
                            editor.parentEntity = nullptr;
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
                        if (item == "Make Independent")
                        {
                            MakeIndependent(editor, editor.selectedEntity);
                        }
                        if (item == "Make Child")
                        {
                            if (editor.parentEntity && editor.selectedEntity)
                            {
                                ChangeParent(editor, editor.selectedEntity, editor.parentEntity);
                            }
                        }
                        if (item == "Make Parent")
                        {
                            if (editor.selectedEntity) editor.parentEntity = editor.selectedEntity;
                        }
                        if (item == "Make Prefab")
                        {
                            if (editor.selectedEntity) editor.entityManager.MakePrefab(editor.prefabManager, editor.selectedEntity);
                        }
                        if (item == "Restore Entity")
                        {
                            if (editor.selectedPrefab) 
                            {
                                editor.entityManager.MakePrefab(editor.entityManager, editor.selectedPrefab, true);
                            }
                        }
                        if (item == "Update Prefab")
                        {
                            editor.entityManager.UpdatePrefab(editor.prefabManager, editor.selectedEntity);
                        }
                        if (item == "Remove")
                        {
                            if (editor.selectedEntity)
                            {
                                editor.Save();
                                if (editor.parentEntity && editor.parentEntity->id() == editor.selectedEntity->id() && editor.parentEntity->tag() == editor.selectedEntity->tag())
                                {
                                    editor.parentEntity = nullptr;
                                }
                                DeleteEntity(editor, editor.selectedEntity);
                                editor.selectedEntity = nullptr;
                            }
                            if (editor.selectedPrefab)
                            {
                                editor.Save();
                                editor.prefabManager.DeleteEntity(editor.selectedPrefab);
                                editor.selectedPrefab = nullptr;
                            }
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
            if (ImGui::BeginMenu("ScriptManager"))
            {
                for (const auto& item : scriptList)
                {
                    if (item == "Rebuild" && ImGui::BeginMenu("Rebuild"))
                    {
                        for (const auto& scriptItem : editor.scriptManager.environmentNames)
                        {
                            if (ImGui::MenuItem(scriptItem.c_str()))
                            {
                                editor.scriptManager.ResetScript(editor, (scriptItem + ".lua"), editor.scriptManager.scriptsDirectoryMap[scriptItem]);
                            }
                        }
                        ImGui::EndMenu();
                    }
                }
                ImGui::EndMenu();
            }
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
        entity->addComponent<CBoxRender>();
    }
    if (type == "Circle")
    {
        std::string name = "Circle" + std::to_string(entity->id());
        entity->addComponent<CName>(name);
        entity->addComponent<CCircleRender>();
    }
}

void EngineSettings::LoadFile(Editor& editor)
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath); // Open a dialog to choose a file
    std::stringstream iss;
    if (result == NFD_OKAY) // If the user selected a file
    {
        iss << "User selected file: " << outPath;
        editor.ConsoleText(iss.str());
        
        fs::path sourceFile(outPath);
        if (sourceFile.extension().string() == ".json") editor.saveFile = sourceFile.string();
        else editor.ConsoleText("Invalid Scene File");
        iss.str("");
    }
    else if (result == NFD_CANCEL) // If the user canceled the file selection
    {
        editor.ConsoleText("User canceled the operation.");
    }
    else // If there was an error
    {
        iss << "Error: " << NFD_GetError();
        editor.ConsoleText(iss.str());
        iss.str("");
    }
    // Free the memory allocated by nfd
    if (outPath)
    {
        free(outPath);
    }
}

void EngineSettings::MakeFile(Editor& editor)
{
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(nullptr, &outPath); // Select Folder
    std::stringstream iss;
    if (result == NFD_OKAY) // If the user selected a folder
    {
        fs::path directory(outPath);
        if (fs::is_directory(directory))
        {
            fs::path jsonFile = directory / "New_Scene.json";
            std::ofstream file(jsonFile);
            if (file.is_open())
            {
                file.close();

                editor.ConsoleText("File created: " + jsonFile.string());
                editor.saveFile = jsonFile.string(); // Save the file path
            }
            else
            {
                editor.ConsoleText("Failed to create file: " + jsonFile.string());
            }
        }
        else
        {
            editor.ConsoleText("Invalid directory " + directory.string());
        }
    }
    else if (result == NFD_CANCEL) // If the user canceled the file selection
    {
        editor.ConsoleText("User canceled the operation.");
    }
    else // If there was an error
    {
        iss << "Error: " << NFD_GetError();
        editor.ConsoleText(iss.str());
        iss.str("");
    }
    // Free the memory allocated by nfd
    if (outPath)
    {
        free(outPath);
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
        editor.ConsoleText(iss.str());
        iss.str("");
        // You can use fs to copy the file to the desired directory
        fs::path sourceFile(outPath);
        fs::path targetDirectory = editor.currentDirectory; // Your current directory

        try
        {
            fs::copy(sourceFile, targetDirectory / sourceFile.filename(),
                fs::copy_options::overwrite_existing);
            editor.ConsoleText("File imported successfully.");
        }
        catch (const fs::filesystem_error& e)
        {
            iss << "Error copying file: " << e.what();
            editor.ConsoleText(iss.str());
            iss.str("");
        }
    }
    else if (result == NFD_CANCEL) // If the user canceled the file selection
    {
        editor.ConsoleText("User canceled the operation.");
    }
    else // If there was an error
    {
        iss << "Error: " << NFD_GetError();
        editor.ConsoleText(iss.str());
        iss.str("");
    }
    // Free the memory allocated by nfd
    if (outPath)
    {
        free(outPath);
    }
}

void EngineSettings::ChangeParent(Editor& editor, std::shared_ptr<Entity>& entity, const std::shared_ptr<Entity>& parent)
{
    if (parent == nullptr) return;
    if (parent->id() == entity->id() && parent->tag() == entity->tag()) return;
    if (entity->hasComponent<CChildren>()) {
        for (auto& c : entity->getComponent<CChildren>().children)
        {
            if (c.first == parent->id() && c.second == parent->tag()) return;
        }
    }
    MakeIndependent(editor, entity);
    auto& parentTrans = parent->getComponent<CTransform>();
    entity->addComponent<CParent>(parent->id(), parent->tag(), parentTrans.pos, parentTrans.scale, parentTrans.angle);
    if (!parent->hasComponent<CChildren>()) { parent->addComponent<CChildren>(); }
    parent->getComponent<CChildren>().children.push_back({ entity->id(), entity->tag() });
}

void EngineSettings::MakeIndependent(Editor& editor, const std::shared_ptr<Entity>& entity)
{
    if (entity->hasComponent<CParent>())
    {
        auto& eParent = entity->getComponent<CParent>();
        auto& eParentEntity = editor.entityManager.getEntity(eParent.id);
        auto& ePPChildren = eParentEntity->getComponent<CChildren>().children;
        auto& ePPChildEntities = eParentEntity->getComponent<CChildren>().childEntities;
        auto it = std::find(ePPChildren.begin(), ePPChildren.end(), std::make_pair(entity->id(), entity->tag()));
        if (it != ePPChildren.end()) {
            auto& value = it->first;
            ePPChildEntities.erase(value);
            ePPChildren.erase(it);
        }
        if (ePPChildren.empty()) eParentEntity->removeComponent<CChildren>();
        entity->removeComponent<CParent>();
    }
}

void EngineSettings::DeleteEntity(Editor& editor, const std::shared_ptr<Entity>& entity)
{
    if (!entity) return;
    MakeIndependent(editor, entity);
    if (entity->hasComponent<CChildren>())
    {
        auto& eChildren = entity->getComponent<CChildren>();
        for (int i = 0; i < eChildren.children.size(); i++)
        {
            auto& c = eChildren.children[i];
            if (editor.parentEntity != nullptr && editor.parentEntity->id() == c.first && editor.parentEntity->tag() == c.second)
            {
                editor.parentEntity = nullptr;
            }
            auto& cEntity = editor.entityManager.getEntity(c.first);
            //std::cout << "Still deleting" << std::endl;
            DeleteEntity(editor, cEntity);
            i--;
        }
    }
    editor.entityManager.destroyEntity(entity);
}