#include "Inspector.h"
#include "Editor.h"

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
}