#include "RenderModifier.h"
#include "Editor.h"

void RenderModifier::Init(Editor& editor)
{

}
void RenderModifier::Update(Editor& editor)
{
	for (auto& e : editor.entityManager.getEntities())
	{
		SetTransform(e);
		if (e->hasComponent<CAnimation>()) e->getComponent<CAnimation>().animation.update();
	}

	/*if (ImGui::IsKeyPressed(ImGuiKey_E))
	{
		if(editor.selectedEntity) editor.selectedEntity->getComponent<CAnimation>();
	}*/
}

void RenderModifier::SetTransform(std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CRectangleShape>())
	{
		auto& Shape = entity->getComponent<CRectangleShape>();
		if (entity->hasComponent<CTransform>())
		{
			auto& Trans = entity->getComponent<CTransform>();
			Shape.rectangle.setPosition(Trans.pos.x, Trans.pos.y);
			Shape.rectangle.setScale(Trans.scale.x, Trans.scale.y);
			Shape.rectangle.setRotation(Trans.angle);
		}
	}
	if (entity->hasComponent<CCircleShape>())
	{
		auto& Shape = entity->getComponent<CCircleShape>();
		if (entity->hasComponent<CTransform>())
		{
			auto& Trans = entity->getComponent<CTransform>();
			Shape.circle.setPosition(Trans.pos.x, Trans.pos.y);
			Shape.circle.setScale(Trans.scale.x, Trans.scale.y);
			Shape.circle.setRotation(Trans.angle);
		}
	}

	if (entity->hasComponent<CAnimation>())
	{
		auto& Shape = entity->getComponent<CAnimation>();
		if (entity->hasComponent<CTransform>())
		{
			auto& Trans = entity->getComponent<CTransform>();
			Shape.animation.getSprite().setPosition(Trans.pos.x, Trans.pos.y);
			Shape.animation.getSprite().setScale(Trans.scale.x, Trans.scale.y);
			Shape.animation.getSprite().setRotation(Trans.angle);
		}
	}

}