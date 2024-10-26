#include "RenderModifier.h"
#include "Editor.h"

void RenderModifier::Init(Editor& editor)
{
	auto entity = editor.entityManager.addEntity("Test");

	entity->addComponent<CName>("t1");
	entity->addComponent<CTransform>(Vec2(300, 400));
	entity->addComponent<CBoxRender>();
	entity->addComponent<CSize>();
}
void RenderModifier::Update(Editor& editor)
{
	for (auto& e : editor.entityManager.getEntities())
	{
		SetTransform(e);
		InfluenceBoundingBox(e);
		ParentChild(editor, e);
		if (e->hasComponent<CAnimation>()) e->getComponent<CAnimation>().animation.update();
		if (editor.gameMode) editor.physicsManager.Update(editor.entityManager, e);
		if (e->hasAnyScriptable()) 
		{
			editor.scriptManager.CompileEntityEnvironment(editor, e);
			editor.scriptManager.ExecuteEntityScripts(editor, e);
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_E))
	{
		if (editor.selectedEntity) 
		{
			if (editor.selectedEntity->hasComponent<CChildren>()) {
				for (auto& c : editor.selectedEntity->getComponent<CChildren>().children)
				{
					editor.ConsoleText(std::to_string(c.first) + c.second);
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
		auto& Size = entity->getComponent<CSize>();
		Trans.prevPos = Trans.pos;

		Trans.scale.x = std::max(Trans.scale.x, minScale);
		Trans.scale.y = std::max(Trans.scale.y, minScale);

		if (entity->hasComponent<CBoxRender>())
		{
			auto& Shape = entity->getComponent<CBoxRender>();
			auto Shapesize = Shape.rectangle.getGlobalBounds().getSize();
			Shape.rectangle.setPosition(Trans.pos.x, Trans.pos.y);
			Shape.rectangle.setScale(Trans.scale.x, Trans.scale.y);
			Shape.rectangle.setRotation(Trans.angle);
			Shape.rectangle.setFillColor(Shape.fillColor);
			Shape.rectangle.setOutlineColor(Shape.outlineColor);
			Shape.rectangle.setOutlineThickness(Shape.outlineThickness);
			Size.size = Vec2(Shapesize.x, Shapesize.y);
			
		}
		if (entity->hasComponent<CCircleRender>())
		{
			auto& Shape = entity->getComponent<CCircleRender>();
			auto Shapesize = Shape.circle.getGlobalBounds().getSize();
			
			Shape.circle.setPosition(Trans.pos.x, Trans.pos.y);
			Shape.circle.setScale(Trans.scale.x, Trans.scale.y);
			Shape.circle.setRotation(Trans.angle);
			Shape.circle.setFillColor(Shape.fillColor);
			Shape.circle.setOutlineColor(Shape.outlineColor);
			Shape.circle.setOutlineThickness(Shape.outlineThickness);
			Shape.circle.setPointCount(Shape.pointCount);
			Size.size = Vec2(Shapesize.x, Shapesize.y);
			
		}

		if (entity->hasComponent<CAnimation>())
		{
			auto& Shape = entity->getComponent<CAnimation>();
			auto Shapesize = Shape.animation.getSprite().getGlobalBounds().getSize();

			Shape.animation.getSprite().setPosition(Trans.pos.x, Trans.pos.y);
			Shape.animation.getSprite().setScale(Trans.scale.x, Trans.scale.y);
			Shape.animation.getSprite().setRotation(Trans.angle);
			Shape.animation.getSprite().setColor(Shape.fillColor);
			Size.size = Vec2(Shapesize.x, Shapesize.y);
		}

		if (entity->hasComponent<CCamera>())
		{
			auto& Cam = entity->getComponent<CCamera>();
			Cam.camera.setPosition(Trans.pos);
			Cam.camera.setScale(Trans.scale);
			Cam.camera.setAngle(Trans.angle);
		}
	}
	

}

void RenderModifier::InfluenceBoundingBox(std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CBoxCollider>() || entity->hasComponent<CCircleCollider>())
	{
		auto& eTrans = entity->getComponent<CTransform>();
		auto& size = entity->getComponent<CSize>().size;
		if (entity->hasComponent<CBoxCollider>())
		{
			auto& box = entity->getComponent<CBoxCollider>();
			box.size = size;
			box.gSize = box.size + box.offset;
		}
		if (entity->hasComponent<CCircleCollider>())
		{
			auto& circle = entity->getComponent<CCircleCollider>();
			float diameter = size.length();
			if (entity->hasComponent<CCircleRender>())
			{
				diameter = std::max(size.x, size.y);
			}
			circle.radius = diameter / 2.0f;
			circle.gRadius = circle.radius + circle.offset;
		}
	}
}

void RenderModifier::ParentChild(Editor& editor, std::shared_ptr<Entity>& entity)
{
	if (entity->hasComponent<CChildren>())
	{
		auto& eTrans = entity->getComponent<CTransform>();
		auto& eChildren = entity->getComponent<CChildren>().children;
		auto& eChildCopies = entity->getComponent<CChildren>().childEntities;
		for (auto& c : eChildren)
		{
			auto& child = editor.entityManager.getEntity(c.first);

			//std::cout << "is null " << (child == nullptr) << std::endl;

			if (child->hasComponent<CTransform>())
			{
				
				auto& cParent = child->getComponent<CParent>();
				auto& cTrans = child->getComponent<CTransform>();
				if (ImGui::IsKeyPressed(ImGuiKey_N))
				{
					editor.ConsoleText(eTrans.pos.toString() + " " + cTrans.pos.toString() + " " + std::to_string(child->id()));
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

				//eChildCopies[child->id()] = editor.entityManager.MakeEntityCopy(child);
			}
		}
	}
}