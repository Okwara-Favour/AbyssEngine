#include "RenderModifier.h"
#include "Editor.h"

void RenderModifier::Init(Editor& editor)
{
	auto entity = editor.entityManager.addEntity("Test");

	entity->addComponent<CName>("t1");
	entity->addComponent<CTransform>(Vec2(300, 400));
	entity->addComponent<CRectangleShape>();
	entity->addComponent<CSize>();
}
void RenderModifier::Update(Editor& editor)
{
	for (auto& e : editor.entityManager.getEntities())
	{
		SetTransform(e);
		ParentChild(editor, e);
		if (e->hasComponent<CAnimation>()) e->getComponent<CAnimation>().animation.update();
		if (e->hasAnyScriptable()) editor.scriptManager.ExecuteEntityScripts(editor, e);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_E))
	{
		if (editor.selectedEntity) 
		{
			if (editor.selectedEntity->hasComponent<CChildren>()) {
				for (auto& c : editor.selectedEntity->getComponent<CChildren>().children)
				{
					editor.HandleError(std::to_string(c.first) + c.second);
				}
			}
		}
	}
}

void RenderModifier::SetTransform(std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CTransform>())
	{
		auto& Trans = entity->getComponent<CTransform>();
		Trans.prevPos = Trans.pos;

		Trans.scale.x = std::max(Trans.scale.x, minScale);
		Trans.scale.y = std::max(Trans.scale.y, minScale);

		if (entity->hasComponent<CRectangleShape>())
		{
			auto& Shape = entity->getComponent<CRectangleShape>();
			if (entity->hasComponent<CTransform>())
			{
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
				Shape.animation.getSprite().setPosition(Trans.pos.x, Trans.pos.y);
				Shape.animation.getSprite().setScale(Trans.scale.x, Trans.scale.y);
				Shape.animation.getSprite().setRotation(Trans.angle);
			}
		}
	}
	

}

void RenderModifier::ParentChild(Editor& editor, std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CChildren>())
	{
		auto& eTrans = entity->getComponent<CTransform>();
		auto& eChildren = entity->getComponent<CChildren>().children;
		for (auto& c : eChildren)
		{
			auto& child = editor.entityManager.getEntity(c.first);
			if (child->hasComponent<CTransform>())
			{
				
				auto& cParent = child->getComponent<CParent>();
				auto& cTrans = child->getComponent<CTransform>();
				if (ImGui::IsKeyPressed(ImGuiKey_N))
				{
					editor.HandleError(eTrans.pos.toString() + " " + cTrans.pos.toString() + " " + std::to_string(child->id()));
				}
				Vec2 scale = eTrans.scale / cParent.initialScale;
				cTrans.scale = cTrans.scale.multiply(scale);
				Vec2 relativePos = (cTrans.pos - cParent.initialPosition).multiply(scale);

				float angle = (eTrans.angle - cParent.initialAngle) * std::_Pi_val/180;  // Calculate rotation difference
				float cosTheta = std::cos(angle);
				float sinTheta = std::sin(angle);
				float rotatedX = relativePos.x * cosTheta - relativePos.y * sinTheta;
				float rotatedY = relativePos.x * sinTheta + relativePos.y * cosTheta;

				cTrans.pos = eTrans.pos + Vec2(rotatedX, rotatedY);
				cParent.initialScale = eTrans.scale;
				cParent.initialPosition = eTrans.pos;
				cParent.initialAngle = eTrans.angle;
			}
		}
	}
}