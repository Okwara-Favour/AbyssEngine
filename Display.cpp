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
		if (e->hasComponent<CShape>())
		{
			displayTexture.draw(e->getComponent<CShape>().rectangle);
		}
	}
	entityMouse = false;
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
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && EntityContainsPos(entity, mouseWorldPos))
		{
			editor.Save();
			editor.selectedEntity = entity;
			entityMouse = true;
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && !entityMouse && editor.selectedEntity)
		{
			editor.Save();
			editor.selectedEntity = nullptr;
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
	if (entity->hasComponent<CShape>())
	{
		auto& Shape = entity->getComponent<CShape>();
		return Shape.rectangle.getGlobalBounds().contains(pos);
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
		if (entity->hasComponent<CShape>())
		{
			auto rect = entity->getComponent<CShape>().rectangle;
			rect.setOutlineThickness(2);
			rect.setOutlineColor(sf::Color::Blue);
			rect.setFillColor(sf::Color::Transparent);
			displayTexture.draw(rect);
			return;
		}
		auto& Size = entity->getComponent<CSize>();
		Size.size = Vec2(2, 2);
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
			if (ImGui::IsKeyPressed(key.first))
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
				ImGui::InputFloat("TranslateFactor", &translateFactor);
				ImGui::InputFloat("ScaleFactor", &scaleFactor);
				ImGui::InputFloat("RotateFactor", &rotateFactor);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Translate"))
			{
				if (ImGui::Button("Up"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.y = -translateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Down"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.y =  translateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Left"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.x = -translateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Right"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		vel.x =  translateFactor;

				ImGui::SameLine();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scale"))
			{
				if (ImGui::Button("ScaleW+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.x =  scaleFactor;

				ImGui::SameLine();
				if (ImGui::Button("ScaleH+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.y =  scaleFactor;

				if (ImGui::Button("ScaleW-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.x = -scaleFactor;
				ImGui::SameLine();
				if (ImGui::Button("ScaleH-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale.y = -scaleFactor;

				if (ImGui::Button("Scale+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale = Vec2(scaleFactor, scaleFactor);

				ImGui::SameLine();
				if (ImGui::Button("Scale-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		scale = Vec2(-scaleFactor, -scaleFactor);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Rotate"))
			{
				if (ImGui::Button("Rotate+"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		angle = rotateFactor;
				
				ImGui::SameLine();
				if (ImGui::Button("Rotate-"));
				if (ImGui::IsItemActivated()) hasClicked = true;
				if (ImGui::IsItemActive())		angle = -rotateFactor;
				
				ImGui::EndMenu();
			}
			if (editor.selectedEntity && editor.selectedEntity->hasComponent<CTransform>())
			{
				auto& Trans = editor.selectedEntity->getComponent<CTransform>();
				Trans.pos += vel;
				Trans.scale += scale;
				Trans.angle += angle;
			}
			if (hasClicked)
			{
				editor.Save();
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMenuBar();
}