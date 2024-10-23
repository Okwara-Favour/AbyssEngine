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
        handleTags(editor);
        displayComponents(editor);
        handleComponents(editor);
	}
    ImGui::PopItemWidth();
	ImGui::End();

    if (ImGui::IsKeyPressed(ImGuiKey_E))
    {
        editor.ConsoleText(std::to_string(editor.animationMap.size()));
        /*
        * for (auto& e : textureMap)
        {
            editor.ConsoleText(e.first);
        }
        */
    }
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
                if (component == "Renderer")
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CBoxRender>();
                }
                if (component == "BoxCollider")
                {
                    editor.Save();
                    auto& size = editor.selectedEntity->getComponent<CSize>().size;
                    editor.selectedEntity->addComponent<CBoxCollider>(size.x, size.y);
                }
                if (component == "CircleCollider")
                {
                    editor.Save();
                    auto& size = editor.selectedEntity->getComponent<CSize>().size;
                    float diameter = size.length();
                    if (editor.selectedEntity->hasComponent<CCircleRender>())
                    {
                        diameter = std::max(size.x, size.y);
                    }
                    editor.selectedEntity->addComponent<CCircleCollider>(diameter/2);
                }
                if (component == "Camera")
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CCamera>();
                }
            }
        }
        for (const auto& scriptName : editor.scriptManager.environmentNames)
        {
            if (ImGui::Selectable(scriptName.c_str()))
            {
                editor.Save();
                editor.selectedEntity->addScriptable(Scriptable(scriptName));
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
                    if (editor.selectedEntity->hasComponent<CBoxRender>()) editor.selectedEntity->removeComponent<CBoxRender>();
                    if (editor.selectedEntity->hasComponent<CCircleRender>())editor.selectedEntity->removeComponent<CCircleRender>();
                    if (editor.selectedEntity->hasComponent<CAnimation>())editor.selectedEntity->removeComponent<CAnimation>();
                }
                if (component == "BoxCollider")
                {
                    editor.Save();
                    editor.selectedEntity->removeComponent<CBoxCollider>();
                }
                if (component == "CircleCollider")
                {
                    editor.Save();
                    editor.selectedEntity->removeComponent<CCircleCollider>();
                }
                if (component == "Camera")
                {
                    editor.Save();
                    editor.selectedEntity->removeComponent<CCamera>();
                }
            }
        }

        for (const auto& scriptName : editor.scriptManager.environmentNames)
        {
            if (ImGui::Selectable(scriptName.c_str()))
            {
                editor.Save();
                editor.selectedEntity->removeScriptable(scriptName);
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

    if (editor.selectedEntity->hasComponent<CBoxRender>() || editor.selectedEntity->hasComponent<CCircleRender>()
        || editor.selectedEntity->hasComponent<CAnimation>())
    {
        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Nothing to see here yet");
            std::string modelName = "None";
            if (editor.selectedEntity)
            {
                if (editor.selectedEntity->hasComponent<CBoxRender>()) modelName = "default_rectangle";
                if (editor.selectedEntity->hasComponent<CCircleRender>()) modelName = "default_circle";
                if (editor.selectedEntity->hasComponent<CAnimation>()) modelName = editor.selectedEntity->getComponent<CAnimation>().animation.getName();
            }
            if (ImGui::BeginCombo("Model", modelName.c_str()))
            {
                for (const auto& model : editor.animationMap)
                {

                    if (ImGui::Selectable(model.first.c_str()))
                    {
                        if (model.first == "default_rectangle")
                        {
                            if (modelName != "default_rectangle") editor.Save(), editor.selectedEntity->addComponent<CBoxRender>();
                            if (modelName == "default_circle")  editor.selectedEntity->removeComponent<CCircleRender>();
                            if (modelName != "default_circle" && modelName != "default_rectangle") editor.selectedEntity->removeComponent<CAnimation>();
                        }
                        else if (model.first == "default_circle")
                        {
                            if (modelName != "default_circle") editor.Save(), editor.selectedEntity->addComponent<CCircleRender>();
                            if (modelName == "default_rectangle") editor.selectedEntity->removeComponent<CBoxRender>();
                            if (modelName != "default_circle" && modelName != "default_rectangle") editor.selectedEntity->removeComponent<CAnimation>();
                        }
                        else
                        {
                            if (modelName != model.second.getName()) editor.Save(), editor.selectedEntity->addComponent<CAnimation>(model.second);
                            if (modelName == "default_rectangle") editor.selectedEntity->removeComponent<CBoxRender>();
                            if (modelName == "default_circle") editor.selectedEntity->removeComponent<CCircleRender>();
                        }
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
            bool hasBox = editor.selectedEntity->hasComponent<CBoxRender>();
            bool hasCircle = editor.selectedEntity->hasComponent<CCircleRender>();
            bool hasAnim = editor.selectedEntity->hasComponent<CAnimation>();

            ImGui::PushItemWidth(200);

            auto& ShapeColor =  hasBox ? editor.selectedEntity->getComponent<CBoxRender>().fillColor :
                                hasCircle ? editor.selectedEntity->getComponent<CCircleRender>().fillColor :
                                editor.selectedEntity->getComponent<CAnimation>().fillColor;

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
                auto& ShapeOutlineColor = hasBox ? editor.selectedEntity->getComponent<CBoxRender>().outlineColor :
                    editor.selectedEntity->getComponent<CCircleRender>().outlineColor;

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
                auto& OutlineThickness = hasBox ? editor.selectedEntity->getComponent<CBoxRender>().outlineThickness :
                    editor.selectedEntity->getComponent<CCircleRender>().outlineThickness;
                //ImGui::InputScalar("Thickness", ImGuiDataType_U64, &OutlineThickness);
                ImGui::InputFloat("Thickness", &OutlineThickness);
                ImGui::Separator();
            }
            if (hasCircle)
            {
                auto& PointCount = editor.selectedEntity->getComponent<CCircleRender>().pointCount;
                ImGui::InputScalar("Points", ImGuiDataType_U64, &PointCount);
                ImGui::Separator();
            }
            ImGui::PopItemWidth();
        }
    }

    if (editor.selectedEntity->hasComponent<CCamera>())
    {
        auto& circle = editor.selectedEntity->getComponent<CCamera>();

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(200);
            auto& Cam = editor.selectedEntity->getComponent<CCamera>().camera;
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

    if (editor.selectedEntity->hasComponent<CBoxCollider>())
    {
        auto& box = editor.selectedEntity->getComponent<CBoxCollider>();

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

    if (editor.selectedEntity->hasComponent<CCircleCollider>())
    {
        auto& circle = editor.selectedEntity->getComponent<CCircleCollider>();

        if (ImGui::CollapsingHeader("Circle Collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat("Radius Offset", &circle.offset);
            ImGui::Separator(); // Adds a horizontal line separator
            ImGui::Text("Radius: %f", circle.gRadius);
            ImGui::Separator(); // Adds a horizontal line separator
        }
    }

    if (editor.selectedEntity->hasAnyScriptable())
    {
        for (auto& script : editor.selectedEntity->m_scriptables)
        {
            if (editor.scriptManager.hasEnvironment(script.first))
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
                            varValue = std::make_any<int>(intValue);
                        }
                        else if (varValue.type() == typeid(unsigned int))
                        {
                            unsigned int uintValue = std::any_cast<unsigned int>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_U32, &uintValue);
                            varValue = std::make_any<unsigned int>(uintValue);
                        }
                        else if (varValue.type() == typeid(float))
                        {
                            float floatValue = std::any_cast<float>(varValue);
                            ImGui::InputFloat(k.first.c_str(), &floatValue);
                            varValue = std::make_any<float>(floatValue);
                        }
                        else if (varValue.type() == typeid(double))
                        {
                            double doubleValue = std::any_cast<double>(varValue);
                            ImGui::InputDouble(k.first.c_str(), &doubleValue);
                            varValue = std::make_any<double>(doubleValue);
                        }
                        else if (varValue.type() == typeid(long))
                        {
                            long longValue = std::any_cast<long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_S64, &longValue);
                            varValue = std::make_any<long>(longValue);
                        }
                        else if (varValue.type() == typeid(unsigned long))
                        {
                            unsigned long ulongValue = std::any_cast<unsigned long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_U64, &ulongValue);
                            varValue = std::make_any<unsigned long>(ulongValue);
                        }
                        else if (varValue.type() == typeid(long long))
                        {
                            long long longlongValue = std::any_cast<long long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_S64, &longlongValue);
                            varValue = std::make_any<long long>(longlongValue);
                        }
                        else if (varValue.type() == typeid(unsigned long long))
                        {
                            unsigned long long ulonglongValue = std::any_cast<unsigned long long>(varValue);
                            ImGui::InputScalar(k.first.c_str(), ImGuiDataType_U64, &ulonglongValue);
                            varValue = std::make_any<unsigned long long>(ulonglongValue);
                        }
                        else if (varValue.type() == typeid(std::string))
                        {
                            std::string strValue = std::any_cast<std::string>(varValue);
                            std::array<char, 256> buffer;
                            std::size_t strLength = strValue.copy(buffer.data(), buffer.size() - 1);
                            buffer[strLength] = '\0';
                            ImGui::InputText(k.first.c_str(), buffer.data(), buffer.size());
                            varValue = std::make_any<std::string>(buffer.data());
                        }
                        else if (varValue.type() == typeid(Vec2))
                        {
                            Vec2 vec2Value = std::any_cast<Vec2>(varValue);
                            float vecArray[2] = { vec2Value.x, vec2Value.y };
                            ImGui::InputFloat2(k.first.c_str(), vecArray);
                            varValue = std::make_any<Vec2>(Vec2(vecArray[0], vecArray[1]));
                        }
                        ImGui::Separator();
                    }
                }
            }
        }
    }
}