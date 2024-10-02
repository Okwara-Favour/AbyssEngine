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
        editor.HandleError(std::to_string(editor.animationMap.size()));
        /*
        * for (auto& e : textureMap)
        {
            editor.HandleError(e.first);
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
                        if (varValue.type() == typeid(std::string))
                        {
                            std::string strValue = std::any_cast<std::string>(varValue);
                            std::array<char, 256> buffer;
                            std::size_t strLength = strValue.copy(buffer.data(), buffer.size() - 1);
                            buffer[strLength] = '\0';
                            ImGui::InputText(k.first.c_str(), buffer.data(), buffer.size());
                            varValue = std::make_any<std::string>(buffer.data());
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