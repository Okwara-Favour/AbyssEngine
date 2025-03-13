#include "Inspector.h"
#include "Editor.h"
#include <typeinfo>

void Inspector::Init(Editor& editor)
{
    editor.animationMap["default_rectangle"];
    editor.animationMap["default_circle"];
}
void Inspector::Update(Editor& editor)
{
	ImGui::Begin("Inspector");
    ImGui::PushItemWidth(100);
	if (editor.selectedEntity)
	{
        ImGui::Text("ID: %d", editor.selectedEntity->id());
        ImGui::Separator();
        if (editor.selectedEntity->hasComponent<CParent>())
        {
            ImGui::Text("Parent: %d", editor.selectedEntity->getComponent<CParent>().id);
            ImGui::Separator();
        }
        if (editor.selectedEntity->prefabData().first)
        {
            ImGui::Text("Prefab-Link: %d", editor.selectedEntity->prefabData().second);
            ImGui::Separator();
        }
        handleTags(editor, editor.selectedEntity);
        displayComponents(editor, editor.selectedEntity);
        handleComponents(editor, editor.selectedEntity);
	}
    if (editor.selectedPrefab && !editor.selectedEntity)
    {
        ImGui::Text("ID: %d", editor.selectedPrefab->id());
        ImGui::Separator();
        if (editor.selectedPrefab->hasComponent<CParent>())
        {
            ImGui::Text("Parent: %d", editor.selectedPrefab->getComponent<CParent>().id);
            ImGui::Separator();
        }
        if (editor.selectedPrefab->prefabData().first)
        {
            ImGui::Text("Prefab-Link: %d", editor.selectedPrefab->prefabData().second);
            ImGui::Separator();
        }
        handleTags(editor, editor.selectedPrefab);
        displayComponents(editor, editor.selectedPrefab);
        handleComponents(editor, editor.selectedPrefab);
    }
    ImGui::PopItemWidth();
	ImGui::End();

    if (ImGui::IsKeyPressed(ImGuiKey_F))
    {
        editor.ConsoleText(std::to_string(editor.animationMap.size()));
        
        if (editor.selectedPrefab)
        {
            if (editor.selectedPrefab->hasComponent<CChildren>())
            {
                editor.ConsoleText("Yes");
                for (auto& e : editor.selectedPrefab->getComponent<CChildren>().childEntities)
                {
                    auto child = std::any_cast<std::shared_ptr<Entity>>(e.second);
                    editor.ConsoleText(child->getComponent<CName>().name);
                }
            }
            
        }
        
        
    }
}

void Inspector::handleTags(Editor& editor, std::shared_ptr<Entity>& selected)
{
    if (ImGui::BeginCombo("Tag", selected->tag().c_str()))
    {
        for (const auto& tag : Tags)
        {
            if (ImGui::Selectable(tag.c_str()))
            {
                if (selected->tag() != tag) {
                    editor.Save();
                    editor.entityManager.changeTag(selected, tag);
                }
            }
        }
        ImGui::EndCombo();
    }
}

void Inspector::handleComponents(Editor& editor, std::shared_ptr<Entity>& selected)
{
    ImGui::PushItemWidth(25);
    if (ImGui::BeginCombo("Add Component", " "))
    {
        for (const auto& component : CList)
        {
            if (ImGui::Selectable(component.c_str()))
            {
                if (component == "Renderer")
                {
                    editor.Save();
                    selected->addComponent<CBoxRender>();
                }
                if (component == "BoxCollider")
                {
                    editor.Save();
                    auto& size = selected->getComponent<CSize>().size;
                    selected->addComponent<CBoxCollider>(size.x, size.y);
                }
                if (component == "CircleCollider")
                {
                    editor.Save();
                    auto& size = selected->getComponent<CSize>().size;
                    float diameter = size.length();
                    if (selected->hasComponent<CCircleRender>())
                    {
                        diameter = std::max(size.x, size.y);
                    }
                    selected->addComponent<CCircleCollider>(diameter / 2);
                }
                if (component == "Camera")
                {
                    editor.Save();
                    selected->addComponent<CCamera>();
                }
            }
        }
        for (const auto& scriptName : editor.scriptManager.environmentNames)
        {
            if (ImGui::Selectable(scriptName.c_str()))
            {
                editor.Save();
                selected->addScriptable(Scriptable(scriptName));
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
                if (component == "Renderer")
                {
                    editor.Save();
                    if (selected->hasComponent<CBoxRender>()) selected->removeComponent<CBoxRender>();
                    if (selected->hasComponent<CCircleRender>())selected->removeComponent<CCircleRender>();
                    if (selected->hasComponent<CAnimation>())selected->removeComponent<CAnimation>();
                }
                if (component == "BoxCollider")
                {
                    editor.Save();
                    selected->removeComponent<CBoxCollider>();
                }
                if (component == "CircleCollider")
                {
                    editor.Save();
                    selected->removeComponent<CCircleCollider>();
                }
                if (component == "Camera")
                {
                    editor.Save();
                    selected->removeComponent<CCamera>();
                }
            }
        }

        for (const auto& scriptName : editor.scriptManager.environmentNames)
        {
            if (ImGui::Selectable(scriptName.c_str()))
            {
                editor.Save();
                selected->removeScriptable(scriptName);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
}

void Inspector::displayComponents(Editor& editor, std::shared_ptr<Entity>& selected)
{
    if (selected->hasComponent<CTransform>())
    {
        auto& trans = selected->getComponent<CTransform>();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& trans = selected->getComponent<CTransform>();
            ImGui::Text("Position");
            ImGui::InputFloat("PX", &trans.pos.x);
            ImGui::SameLine();
            ImGui::InputFloat("PY", &trans.pos.y);
            ImGui::Separator(); // Adds a horizontal line separator
            ImGui::Text("Scale");
            ImGui::InputFloat("SX", &trans.scale.x);
            ImGui::SameLine();
            ImGui::InputFloat("SY", &trans.scale.y);
            ImGui::Separator();
            ImGui::Text("Rotation");
            ImGui::InputFloat("Angle", &trans.angle);
            ImGui::Separator();
        }
    }

    if (selected->hasComponent<CBoxRender>() || selected->hasComponent<CCircleRender>()
        || selected->hasComponent<CAnimation>())
    {
        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Nothing to see here yet");
            std::string modelName = "None";
            if (selected)
            {
                if (selected->hasComponent<CBoxRender>()) modelName = "default_rectangle";
                if (selected->hasComponent<CCircleRender>()) modelName = "default_circle";
                if (selected->hasComponent<CAnimation>()) modelName = selected->getComponent<CAnimation>().animation.getName();
            }
            if (ImGui::BeginCombo("Model", modelName.c_str()))
            {
                for (const auto& model : editor.animationMap)
                {

                    if (ImGui::Selectable(model.first.c_str()))
                    {
                        if (model.first == "default_rectangle")
                        {
                            if (modelName != "default_rectangle") editor.Save(), selected->addComponent<CBoxRender>();
                            if (modelName == "default_circle")  selected->removeComponent<CCircleRender>();
                            if (modelName != "default_circle" && modelName != "default_rectangle") selected->removeComponent<CAnimation>();
                        }
                        else if (model.first == "default_circle")
                        {
                            if (modelName != "default_circle") editor.Save(), selected->addComponent<CCircleRender>();
                            if (modelName == "default_rectangle") selected->removeComponent<CBoxRender>();
                            if (modelName != "default_circle" && modelName != "default_rectangle") selected->removeComponent<CAnimation>();
                        }
                        else
                        {
                            if (modelName != model.second.getName()) editor.Save(), selected->addComponent<CAnimation>(model.second);
                            if (modelName == "default_rectangle") selected->removeComponent<CBoxRender>();
                            if (modelName == "default_circle") selected->removeComponent<CCircleRender>();
                        }
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
            bool hasBox = selected->hasComponent<CBoxRender>();
            bool hasCircle = selected->hasComponent<CCircleRender>();
            bool hasAnim = selected->hasComponent<CAnimation>();

            ImGui::PushItemWidth(200);

            auto& ShapeColor = hasBox ? selected->getComponent<CBoxRender>().fillColor :
                hasCircle ? selected->getComponent<CCircleRender>().fillColor :
                selected->getComponent<CAnimation>().fillColor;

            // Convert sf::Color to a float array (normalized [0, 1] for ImGui)
            float color[4] = {
                ShapeColor.r / 255.0f,
                ShapeColor.g / 255.0f,
                ShapeColor.b / 255.0f,
                ShapeColor.a / 255.0f
            };

            // ImGui color picker for the fill color
            if (ImGui::ColorEdit4("Fill Color", color))
            {
                // Convert float array back to sf::Color (scaled to [0, 255])
                ShapeColor = sf::Color(
                    static_cast<sf::Uint8>(color[0] * 255),
                    static_cast<sf::Uint8>(color[1] * 255),
                    static_cast<sf::Uint8>(color[2] * 255),
                    static_cast<sf::Uint8>(color[3] * 255)
                );
            }
            ImGui::Separator();
            if (hasBox || hasCircle)
            {
                auto& ShapeOutlineColor = hasBox ? selected->getComponent<CBoxRender>().outlineColor :
                    selected->getComponent<CCircleRender>().outlineColor;

                float outlineColor[4] = {
                    ShapeOutlineColor.r / 255.0f,
                    ShapeOutlineColor.g / 255.0f,
                    ShapeOutlineColor.b / 255.0f,
                    ShapeOutlineColor.a / 255.0f
                };

                // ImGui color picker for the fill color
                if (ImGui::ColorEdit4("Outline Color", outlineColor))
                {
                    // Convert float array back to sf::Color (scaled to [0, 255])
                    ShapeOutlineColor = sf::Color(
                        static_cast<sf::Uint8>(outlineColor[0] * 255),
                        static_cast<sf::Uint8>(outlineColor[1] * 255),
                        static_cast<sf::Uint8>(outlineColor[2] * 255),
                        static_cast<sf::Uint8>(outlineColor[3] * 255)
                    );
                }

                ImGui::Separator();
                auto& OutlineThickness = hasBox ? selected->getComponent<CBoxRender>().outlineThickness :
                    selected->getComponent<CCircleRender>().outlineThickness;
                //ImGui::InputScalar("Thickness", ImGuiDataType_U64, &OutlineThickness);
                ImGui::InputFloat("Thickness", &OutlineThickness);
                ImGui::Separator();
            }
            if (hasCircle)
            {
                auto& PointCount = selected->getComponent<CCircleRender>().pointCount;
                ImGui::InputScalar("Points", ImGuiDataType_U64, &PointCount);
                ImGui::Separator();
            }
            ImGui::PopItemWidth();
        }
    }

    if (selected->hasComponent<CCamera>())
    {
        auto& circle = selected->getComponent<CCamera>();

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(200);
            auto& Cam = selected->getComponent<CCamera>().camera;
            auto& SceneColor = Cam.getColor();

            // Convert sf::Color to a float array (normalized [0, 1] for ImGui)
            float color[4] = {
                SceneColor.r / 255.0f,
                SceneColor.g / 255.0f,
                SceneColor.b / 255.0f,
                SceneColor.a / 255.0f
            };

            // ImGui color picker for the fill color
            if (ImGui::ColorEdit4("Scene Color", color))
            {
                Cam.setColor(
                    static_cast<int>(color[0] * 255),
                    static_cast<int>(color[1] * 255),
                    static_cast<int>(color[2] * 255),
                    static_cast<int>(color[3] * 255)
                );
            }
            ImGui::PopItemWidth();
            ImGui::Separator();
        }
    }

    if (selected->hasComponent<CBoxCollider>())
    {
        auto& box = selected->getComponent<CBoxCollider>();

        if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat("Width Offset", &box.offset.x);
            ImGui::SameLine();
            ImGui::InputFloat("Height Offset", &box.offset.y);
            ImGui::Separator(); // Adds a horizontal line separator
            ImGui::Text("Width: %f", box.gSize.x);
            ImGui::SameLine();
            ImGui::Text("Height: %f", box.gSize.y);
            ImGui::Separator(); // Adds a horizontal line separator
        }
    }

    if (selected->hasComponent<CCircleCollider>())
    {
        auto& circle = selected->getComponent<CCircleCollider>();

        if (ImGui::CollapsingHeader("Circle Collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat("Radius Offset", &circle.offset);
            ImGui::Separator(); // Adds a horizontal line separator
            ImGui::Text("Radius: %f", circle.gRadius);
            ImGui::Separator(); // Adds a horizontal line separator
        }
    }

    if (selected->hasAnyScriptable())
    {
        for (auto& script : selected->m_scriptables)
        {
            if (editor.scriptManager.hasCompiledScript(script.first))
            {
                if (ImGui::CollapsingHeader(script.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto& k : script.second.variableMap)
                    {
                        auto& varValue = k.second;
                        if (varValue.type() == typeid(int))
                        {
                            int intValue = std::any_cast<int>(varValue);
                            ImGui::InputInt(k.first.c_str(), &intValue);
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<int>(intValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(unsigned int))
                        {
                            unsigned int uintValue = std::any_cast<unsigned int>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_U32, &uintValue);
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<unsigned int>(uintValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(float))
                        {
                            float floatValue = std::any_cast<float>(varValue);
                            ImGui::InputFloat(k.first.c_str(), &floatValue);
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<float>(floatValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(double))
                        {
                            double doubleValue = std::any_cast<double>(varValue);
                            ImGui::InputDouble(k.first.c_str(), &doubleValue);
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<double>(doubleValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                            
                        }
                        else if (varValue.type() == typeid(long))
                        {
                            long longValue = std::any_cast<long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_S64, &longValue);
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<long>(longValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(unsigned long))
                        {
                            unsigned long ulongValue = std::any_cast<unsigned long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_U64, &ulongValue);
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<unsigned long>(ulongValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(long long))
                        {
                            long long longlongValue = std::any_cast<long long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_S64, &longlongValue);
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<long long>(longlongValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(unsigned long long))
                        {
                            unsigned long long ulonglongValue = std::any_cast<unsigned long long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_U64, &ulonglongValue);
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<unsigned long long>(ulonglongValue);
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(std::string))
                        {
                            std::string strValue = std::any_cast<std::string>(varValue);
                            std::array<char, 256> buffer;
                            std::size_t strLength = strValue.copy(buffer.data(), buffer.size() - 1);
                            buffer[strLength] = '\0';
                            ImGui::InputText(k.first.c_str(), buffer.data(), buffer.size());
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<std::string>(buffer.data());
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        else if (varValue.type() == typeid(Vec2))
                        {
                            Vec2 vec2Value = std::any_cast<Vec2>(varValue);
                            float vecArray[2] = { vec2Value.x, vec2Value.y };
                            ImGui::InputFloat2(k.first.c_str(), vecArray);
                            
                            if (ImGui::IsItemDeactivatedAfterEdit()) {
                                varValue = std::make_any<Vec2>(Vec2(vecArray[0], vecArray[1]));
                                if (editor.gameMode) editor.scriptManager.pushEnvironmentVariables(script.second);
                            }
                        }
                        ImGui::Separator();
                    }
                }
            }
        }
    }
}