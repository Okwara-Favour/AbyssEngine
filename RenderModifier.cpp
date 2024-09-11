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
	}
}

void RenderModifier::SetTransform(std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CShape>())
	{
		auto& Shape = entity->getComponent<CShape>();
		if (entity->hasComponent<CTransform>())
		{
			auto& Trans = entity->getComponent<CTransform>();
			Shape.rectangle.setPosition(Trans.pos.x, Trans.pos.y);
			Shape.rectangle.setScale(Trans.scale.x, Trans.scale.y);
			Shape.rectangle.setRotation(Trans.angle);
		}
	}
}