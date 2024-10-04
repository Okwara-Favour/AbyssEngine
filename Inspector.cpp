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
        if (editor.selectedEntity->hasComponent<CParent>())
        {
            ImGui::Text("Parent: %d", editor.selectedEntity->getComponent<CParent>().id);
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
                if (component == "Transform") 
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CTransform>(Vec2(editor.startPosition.x, editor.startPosition.y));
                }
                if (component == "Renderer")
                {
                    editor.Save();
                    editor.selectedEntity->addComponent<CRectangleShape>();
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
                if (component == "Transform")
                {
                    editor.Save();
                    editor.selectedEntity->removeComponent<CTransform>();
                }
                if (component == "Renderer")
                {
                    editor.Save();
                    if (editor.selectedEntity->hasComponent<CRectangleShape>()) editor.selectedEntity->removeComponent<CRectangleShape>();
                    if (editor.selectedEntity->hasComponent<CCircleShape>())editor.selectedEntity->removeComponent<CCircleShape>();
                    if (editor.selectedEntity->hasComponent<CAnimation>())editor.selectedEntity->removeComponent<CAnimation>();
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
        }
    }

    if (editor.selectedEntity->hasComponent<CRectangleShape>() || editor.selectedEntity->hasComponent<CCircleShape>()
        || editor.selectedEntity->hasComponent<CAnimation>())
    {
        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Nothing to see here yet");
            std::string modelName = "None";
            if (editor.selectedEntity)
            {
                if (editor.selectedEntity->hasComponent<CRectangleShape>()) modelName = "default_rectangle";
                if (editor.selectedEntity->hasComponent<CCircleShape>()) modelName = "default_circle";
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
                            if (modelName != "default_rectangle") editor.Save(), editor.selectedEntity->addComponent<CRectangleShape>();
                            if (modelName == "default_circle")  editor.selectedEntity->removeComponent<CCircleShape>();
                            if (modelName != "default_circle" && modelName != "default_rectangle") editor.selectedEntity->removeComponent<CAnimation>();
                        }
                        else if (model.first == "default_circle")
                        {
                            if (modelName != "default_circle") editor.Save(), editor.selectedEntity->addComponent<CCircleShape>();
                            if (modelName == "default_rectangle") editor.selectedEntity->removeComponent<CRectangleShape>();
                            if (modelName != "default_circle" && modelName != "default_rectangle") editor.selectedEntity->removeComponent<CAnimation>();
                        }
                        else
                        {
                            if (modelName != model.second.getName()) editor.Save(), editor.selectedEntity->addComponent<CAnimation>(model.second);
                            if (modelName == "default_rectangle") editor.selectedEntity->removeComponent<CRectangleShape>();
                            if (modelName == "default_circle") editor.selectedEntity->removeComponent<CCircleShape>();
                        }
                    }
                }
                ImGui::EndCombo();
            }
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
                    }
                }
            }
        }
    }

    /*if (editor.selectedEntity->hasScriptable<Scriptable>())
    {
        for (const auto& script : editor.selectedEntity->m_scriptables)
        {
            std::string cname = typeid(*script).name();
            if (ImGui::CollapsingHeader(cname.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (auto& k : script->variableMap)
                {
                    auto& valuePtr = k.second; // This is a shared_ptr<std::any>
                    // Dereference the shared_ptr to access the std::any
                    const std::any& value = *valuePtr;
                    if (value.type() == typeid(std::reference_wrapper<float>)) {
                        // Perform the cast to retrieve the reference
                        float& floatValue = std::any_cast<std::reference_wrapper<float>>(value).get(); // Get reference
                        ImGui::InputFloat(k.first.c_str(), &floatValue);
                    }
                }
            }
        }
    }*/
}