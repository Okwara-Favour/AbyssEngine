#include "Display.h"
#include "Editor.h"

void Display::Init(Editor& editor)
{
	displayTexture.create(editor.WinSize().x, editor.WinSize().y);
	keyAction[ImGuiKey_V] = "ZOOMIN";
	keyAction[ImGuiKey_B] = "ZOOMOUT";
	keyAction[ImGuiKey_UpArrow] = "MOVEUP";
	keyAction[ImGuiKey_DownArrow] = "MOVEDOWN";
	keyAction[ImGuiKey_LeftArrow] = "MOVELEFT";
	keyAction[ImGuiKey_RightArrow] = "MOVERIGHT";
}
void Display::Update(Editor& editor)
{
	ImGui::Begin("Display", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);

	windowPos = ImGui::GetWindowPos();
	ImVec2 tabSize = ImGui::GetWindowSize();
	contentMin = ImGui::GetWindowContentRegionMin();
	contentMax = ImGui::GetWindowContentRegionMax();
	ImVec2 contentSize = ImVec2(contentMax.x - contentMin.x, contentMax.y - contentMin.y);
	viewPos = ImVec2(contentMin.x + windowPos.x, contentMin.y + windowPos.y);
	viewScale = ImVec2(editor.WinSize().x / contentSize.x, editor.WinSize().y / contentSize.y);

	displayTexture.clear(sf::Color(100,100,100));
	for (auto& e : editor.entityManager.getEntities())
	{
		DisplaySelected(e, editor.selectedEntity);
		EntityMouseDrag(e, editor);
		if (e->hasComponent<CRectangleShape>())
		{
			displayTexture.draw(e->getComponent<CRectangleShape>().rectangle);
		}
		if (e->hasComponent<CCircleShape>())
		{
			displayTexture.draw(e->getComponent<CCircleShape>().circle);
		}
		if (e->hasComponent<CAnimation>())
		{
			displayTexture.draw(e->getComponent<CAnimation>().animation.getSprite());
		}
	}
	if (editor.isMouseInTab() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && !entityClicked && editor.selectedEntity)
	{
		editor.Save();
		editor.selectedEntity = nullptr;
	}
	entityClicked = false;
	editor.startPosition = ImGui::GetWindowViewport()->GetCenter();
	ImGui::Image((void*)(intptr_t)displayTexture.getTexture().getNativeHandle(),
		ImVec2(contentSize.x, contentSize.y),
		ImVec2(0, 1), ImVec2(1, 0));
	displayTexture.display();
	HandleKeyActions(editor);
	MenuTab(editor);
	if (ImGui::IsKeyPressed(ImGuiKey_Space))
	{
		std::cout << editor.selectedEntity << std::endl;
	}
	ImGui::End();
}

void Display::EntityMouseDrag(std::shared_ptr<Entity>& entity, Editor& editor)
{
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 mousePosInWindow = ImVec2(mousePos.x - viewPos.x, mousePos.y - viewPos.y);
	sf::Vector2f scaledPos = sf::Vector2f(mousePosInWindow.x * viewScale.x, mousePosInWindow.y * viewScale.y);
	sf::Vector2f mouseWorldPos = WorldPos(scaledPos.x, scaledPos.y);
	
	if (editor.isMouseInTab())
	{
		bool alreadySelected = editor.selectedEntity && entity->id() == editor.selectedEntity->id();
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && EntityContainsPos(entity, mouseWorldPos))
		{
			if(!alreadySelected && !entityClicked) editor.Save();
			editor.selectedEntity = entity;
			entityClicked = true;
		}
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && editor.selectedEntity && entity->id() == editor.selectedEntity->id())
		{
			Vec2 mPos(mouseWorldPos.x, mouseWorldPos.y);
			Vec2& ePos = entity->getComponent<CTransform>().pos;
			ePos = mPos;
		}
	}
}

bool Display::EntityContainsPos(std::shared_ptr<Entity>& entity, sf::Vector2f& pos)
{
	auto& Size = entity->getComponent<CSize>();
	auto& Trans = entity->getComponent<CTransform>();
	if (entity->hasComponent<CRectangleShape>())
	{
		auto& Shape = entity->getComponent<CRectangleShape>();
		return Shape.rectangle.getGlobalBounds().contains(pos);
	}
	if (entity->hasComponent<CCircleShape>())
	{
		auto& Shape = entity->getComponent<CCircleShape>();
		return Shape.circle.getGlobalBounds().contains(pos);
	}
	if (entity->hasComponent<CAnimation>())
	{
		auto& Shape = entity->getComponent<CAnimation>().animation;
		return Shape.getSprite().getGlobalBounds().contains(pos);
	}
	auto minBounds = Trans.pos - (Size.size / 2.0f);
	auto maxBounds = Trans.pos + (Size.size / 2.0f);
	return pos.x >= minBounds.x && pos.x <= maxBounds.x && pos.y >= minBounds.y && pos.y <= maxBounds.y;
}
sf::Vector2f Display::WorldPos(float x, float y)
{
	return displayTexture.mapPixelToCoords(sf::Vector2i(x, y));
}

void Display::DisplaySelected(std::shared_ptr<Entity>& entity, std::shared_ptr<Entity>& selected)
{
	if (selected && entity->id() == selected->id())
	{
		if (entity->hasComponent<CRectangleShape>())
		{
			auto rect = entity->getComponent<CRectangleShape>().rectangle;
			rect.setOutlineThickness(2);
			rect.setOutlineColor(sf::Color::Blue);
			rect.setFillColor(sf::Color::Transparent);
			displayTexture.draw(rect);
			return;
		}
		if (entity->hasComponent<CCircleShape>())
		{
			auto circ = entity->getComponent<CCircleShape>().circle;
			circ.setOutlineThickness(2);
			circ.setOutlineColor(sf::Color::Blue);
			circ.setFillColor(sf::Color::Transparent);
			displayTexture.draw(circ);
			return;
		}
		if (entity->hasComponent<CAnimation>())
		{
			auto& sprite = entity->getComponent<CAnimation>().animation.getSprite();
			auto bounds = sprite.getGlobalBounds();
			sf::RectangleShape rect(sf::Vector2f(bounds.width, bounds.height));
			rect.setPosition(sprite.getPosition().x, sprite.getPosition().y);
			rect.setOutlineThickness(2);
			rect.setOutlineColor(sf::Color::Blue);
			rect.setFillColor(sf::Color::Transparent);
			rect.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
			displayTexture.draw(rect);
			return;
		}
		auto& Size = entity->getComponent<CSize>();
		Size.size = Vec2(5, 5);
		auto& Trans = entity->getComponent<CTransform>();
		sf::RectangleShape rect(sf::Vector2f(Size.size.x, Size.size.y));
		rect.setOrigin(Size.size.x / 2, Size.size.y / 2);
		rect.setPosition(Trans.pos.x, Trans.pos.y);
		rect.setOutlineThickness(2);
		rect.setOutlineColor(sf::Color::Blue);
		rect.setFillColor(sf::Color::Transparent);
		displayTexture.draw(rect);
	}
}

void Display::HandleKeyActions(Editor& editor)
{
	if (editor.isMouseInTab())
	{
		sf::View view = displayTexture.getView();
		sf::Vector2f viewVel;
		for (auto& key : keyAction)
		{
			if (!editor.gameMode && ImGui::IsKeyPressed(key.first))
			{
				if (key.second == "ZOOMIN")		view.zoom(1.1);
				if (key.second == "ZOOMOUT")	view.zoom(0.9);
				if (key.second == "MOVEUP")		viewVel.y = -1;
				if (key.second == "MOVEDOWN")	viewVel.y =	 1;
				if (key.second == "MOVELEFT")	viewVel.x = -1;
				if (key.second == "MOVERIGHT")	viewVel.x =  1;
			}
		}
		view.move(viewVel);
		displayTexture.setView(view);
	}
}

void Display::MenuTab(Editor& editor)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			sf::View view = displayTexture.getView();
			sf::Vector2f viewVel;
			if (ImGui::Button("Up"));
			if (ImGui::IsItemActive())		viewVel.y = -1;
			ImGui::SameLine();
			if (ImGui::Button("Down"));
			if (ImGui::IsItemActive())		viewVel.y =  1;
			ImGui::SameLine();
			if (ImGui::Button("Left"));
			if (ImGui::IsItemActive())		viewVel.x = -1;
			ImGui::SameLine();
			if (ImGui::Button("Right"));
			if (ImGui::IsItemActive())		viewVel.x =  1;
			ImGui::SameLine();
			if (ImGui::Button("ZOOMIN"));
			if (ImGui::IsItemActive())		view.zoom(1.1);
			ImGui::SameLine();
			if (ImGui::Button("ZOOMOUT"));
			if (ImGui::IsItemActive())		view.zoom(0.9);
			view.move(viewVel);
			displayTexture.setView(view);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Transform"))
		{
			bool hasClicked = false;
			bool hasDeactivated = false;
			Vec2 vel;
			Vec2 scale;
			float angle = 0.0f;
			if (ImGui::BeginMenu("Factors"))
			{
				ImGui::InputFloat("TranslateFactor", &editor.translateFactor);
				ImGui::InputFloat("ScaleFactor", &editor.scaleFactor);
				ImGui::InputFloat("RotateFactor", &editor.rotateFactor);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Translate"))
			{
				if (ImGui::Button("Up"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.y = -editor.translateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Down"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.y =  editor.translateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Left"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.x = -editor.translateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Right"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.x =  editor.translateFactor;

				ImGui::SameLine();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scale"))
			{
				if (ImGui::Button("ScaleW+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.x =  editor.scaleFactor;

				ImGui::SameLine();
				if (ImGui::Button("ScaleH+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.y =  editor.scaleFactor;

				if (ImGui::Button("ScaleW-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.x = -editor.scaleFactor;
				ImGui::SameLine();
				if (ImGui::Button("ScaleH-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.y = -editor.scaleFactor;

				if (ImGui::Button("Scale+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale = Vec2(editor.scaleFactor, editor.scaleFactor);

				ImGui::SameLine();
				if (ImGui::Button("Scale-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale = Vec2(-editor.scaleFactor, -editor.scaleFactor);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Rotate"))
			{
				if (ImGui::Button("Rotate+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		angle = editor.rotateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Rotate-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		angle = -editor.rotateFactor;
				
				ImGui::EndMenu();
			}
			if (clock.getElapsedTime() >= sf::seconds(editor.duration) && editor.selectedEntity && editor.selectedEntity->hasComponent<CTransform>())
			{
				auto& Trans = editor.selectedEntity->getComponent<CTransform>();
				Trans.pos += vel;
				Trans.scale += scale;
				Trans.angle += angle;
				clock.restart();
			}
			if (hasClicked)
			{
				clock.restart();
				if (editor.selectedEntity)
				{
					editor.Save();
					auto& Trans = editor.selectedEntity->getComponent<CTransform>();
					Trans.pos += vel;
					Trans.scale += scale;
					Trans.angle += angle;
				}
			}
			ImGui::EndMenu();
		}
		if (editor.gameMode)
		{
			if (ImGui::Button("Quit"))
			{
				editor.QuitGame();
			}
		}
		else 
		{
			if (ImGui::Button("Play"))
			{
				editor.StartGame();
			}
		}
	}
	ImGui::EndMenuBar();
}