#include "Display.h"
#include "Editor.h"

void Display::Init(Editor& editor)
{
	keyAction[ImGuiKey_V] = "ZOOMIN";
	keyAction[ImGuiKey_B] = "ZOOMOUT";
	keyAction[ImGuiKey_UpArrow] = "MOVEUP";
	keyAction[ImGuiKey_DownArrow] = "MOVEDOWN";
	keyAction[ImGuiKey_LeftArrow] = "MOVELEFT";
	keyAction[ImGuiKey_RightArrow] = "MOVERIGHT";
	mainViewPort = editor.window.getDefaultView();
	//std::cout << mainViewPort.getSize().x << " " << mainViewPort.getSize().y << std::endl;
}
void Display::Update(Editor& editor)
{
	ImGui::Begin("Display", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground);

	windowPos = ImGui::GetWindowPos();
	ImVec2 tabSize = ImGui::GetWindowSize();
	contentMin = ImGui::GetWindowContentRegionMin();
	contentMax = ImGui::GetWindowContentRegionMax();
	ImVec2 contentSize = ImVec2(contentMax.x - contentMin.x, contentMax.y - contentMin.y);
	ImVec2 contentCenter = ImVec2((contentMax.x - contentMin.x)/2.0f, (contentMax.y - contentMin.y)/2.0f);
	const auto& texSize = editor.window.getSize();
	viewPos = ImVec2(contentMin.x + windowPos.x, contentMin.y + windowPos.y);
	viewScale = ImVec2(editor.WinSize().x / texSize.x, editor.WinSize().y / texSize.y);

	float left = viewPos.x / (float)editor.WinSize().x;
	float top = viewPos.y / (float)editor.WinSize().y;
	float width = contentSize.x / (float)editor.WinSize().x;
	float height = contentSize.y / (float)editor.WinSize().y;

	//mainViewPort = editor.window.getDefaultView();
	mainViewPort.setViewport(sf::FloatRect(left, top, width, height));

	editor.window.clear();

	sf::Vector2f rsize(editor.WinSize().x + editor.WinSize().x / 2.0f, editor.WinSize().y + editor.WinSize().y / 2.0f);
	mainRect.setSize(rsize);
	mainRect.setPosition((editor.WinSize().x / 2.0f) + m_stackedMoveChange.x, 
						  (editor.WinSize().y / 2.0f) + m_stackedMoveChange.y);
	mainRect.setOrigin(rsize.x / 2.0f, rsize.y / 2.0f);
	mainRect.setFillColor(viewColor);
	MenuTab(editor);
	HandleKeyActions(editor);

	editor.window.draw(mainRect);
	for (auto& e : editor.entityManager.getEntities())
	{
		DisplaySelected(e, editor);
		EntityMouseDrag(e, editor);
		if (editor.gameMode)
		{
			if (e->hasComponent<CCamera>())
			{
				auto& Cam = e->getComponent<CCamera>();
				Cam.camera.apply(editor.window, Vec2(left, top), Vec2(width, height));
				viewColor = Cam.camera.getColor();
			}
		}
		else
		{
			if (e->hasComponent<CCamera>())
			{
				editor.window.draw(e->getComponent<CCamera>().camera.m_rectangle);
			}
			viewColor = editorViewColor;
			//mainViewPort.setSize(editor.WinSize().x, editor.WinSize().y);
		}
		if (e->hasComponent<CBoxRender>())
		{
			editor.window.draw(e->getComponent<CBoxRender>().rectangle);
		}
		if (e->hasComponent<CCircleRender>())
		{
			editor.window.draw(e->getComponent<CCircleRender>().circle);
		}
		if (e->hasComponent<CAnimation>())
		{
			editor.window.draw(e->getComponent<CAnimation>().animation.getSprite());
		}
	}
	if (editor.isMouseInTab() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && !entityClicked && editor.selectedEntity)
	{
		editor.Save();
		editor.selectedEntity = nullptr;
		editor.selectedPrefab = nullptr;
	}
	//std::cout << texSize.x << " " << texSize.y << std::endl;
	entityClicked = false;
	editor.startPosition = contentCenter;

	editor.window.setView(mainViewPort);

	if (ImGui::IsKeyPressed(ImGuiKey_Space))
	{
		//std::cout << editor.selectedEntity << std::endl;
	}
	ImGui::End();
}

void Display::EntityMouseDrag(std::shared_ptr<Entity>& entity, Editor& editor)
{
	ImVec2 mousePos = ImGui::GetMousePos();
	sf::Vector2f mouseWorldPos = WorldPos(editor, mousePos.x, mousePos.y);

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
	if (entity->hasComponent<CBoxRender>())
	{
		auto& Shape = entity->getComponent<CBoxRender>();
		return Shape.rectangle.getGlobalBounds().contains(pos);
	}
	if (entity->hasComponent<CCircleRender>())
	{
		auto& Shape = entity->getComponent<CCircleRender>();
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
sf::Vector2f Display::WorldPos(Editor& editor, float x, float y)
{
	return editor.window.mapPixelToCoords(sf::Vector2i(x, y));
}

void Display::DisplaySelectedBound(Editor& editor)
{
	auto& selected = editor.selectedEntity;
	if (selected->hasComponent<CBoxCollider>())
	{
		auto& Trans = selected->getComponent<CTransform>();
		auto& box = selected->getComponent<CBoxCollider>();
		sf::RectangleShape rect(sf::Vector2f(box.gSize.x, box.gSize.y));
		rect.setOrigin(box.gSize.x / 2, box.gSize.y / 2);
		rect.setPosition(Trans.pos.x, Trans.pos.y);
		rect.setOutlineThickness(2);
		rect.setOutlineColor(sf::Color::Red);
		rect.setFillColor(sf::Color::Transparent);
		editor.window.draw(rect);
	}
	if (selected->hasComponent<CCircleCollider>())
	{
		auto& Trans = selected->getComponent<CTransform>();
		auto& circle = selected->getComponent<CCircleCollider>();
		sf::CircleShape cir(circle.gRadius);
		cir.setOrigin(circle.gRadius, circle.gRadius);
		cir.setPosition(Trans.pos.x, Trans.pos.y);
		cir.setOutlineThickness(2);
		cir.setOutlineColor(sf::Color::Red);
		cir.setFillColor(sf::Color::Transparent);
		editor.window.draw(cir);
	}
}

void Display::DisplaySelected(std::shared_ptr<Entity>& entity, Editor& editor)
{
	if (editor.selectedEntity && entity->id() == editor.selectedEntity->id())
	{
		if (entity->hasComponent<CBoxRender>())
		{
			auto rect = entity->getComponent<CBoxRender>().rectangle;
			rect.scale(1.25, 1.5);
			rect.setOutlineThickness(2);
			rect.setOutlineColor(sf::Color::Blue);
			rect.setFillColor(sf::Color::Transparent);
			editor.window.draw(rect);
			DisplaySelectedBound(editor);
			return;
		}
		if (entity->hasComponent<CCircleRender>())
		{
			auto circ = entity->getComponent<CCircleRender>().circle;
			circ.scale(1.25, 1.25);
			circ.setOutlineThickness(2);
			circ.setOutlineColor(sf::Color::Blue);
			circ.setFillColor(sf::Color::Transparent);
			editor.window.draw(circ);
			DisplaySelectedBound(editor);
			return;
		}
		if (entity->hasComponent<CAnimation>())
		{
			auto& sprite = entity->getComponent<CAnimation>().animation.getSprite();
			auto bounds = sprite.getGlobalBounds();
			sf::RectangleShape rect(sf::Vector2f(bounds.width, bounds.height));
			rect.scale(1.25, 1.25);
			rect.setPosition(sprite.getPosition().x, sprite.getPosition().y);
			rect.setOutlineThickness(2);
			rect.setOutlineColor(sf::Color::Blue);
			rect.setFillColor(sf::Color::Transparent);
			rect.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
			editor.window.draw(rect);
			DisplaySelectedBound(editor);
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
		editor.window.draw(rect);
		DisplaySelectedBound(editor);
	}
}

void Display::HandleKeyActions(Editor& editor)
{
	if (editor.isMouseInTab())
	{
		sf::Vector2f viewVel;
		for (auto& key : keyAction)
		{
			if (!editor.gameMode && ImGui::IsKeyPressed(key.first))
			{
				if (key.second == "ZOOMIN") {
					sf::RectangleShape crect = mainRect;
					mainRect.scale(0.95, 0.95);
					mainViewPort.zoom(0.95);
				}
				if (key.second == "ZOOMOUT") {
					mainRect.scale(1.05, 1.05);
					mainViewPort.zoom(1.05);
				}
				if (key.second == "MOVEUP")		viewVel.y = -1;
				if (key.second == "MOVEDOWN")	viewVel.y =	 1;
				if (key.second == "MOVELEFT")	viewVel.x = -1;
				if (key.second == "MOVERIGHT")	viewVel.x =  1;
			}
		}
		mainViewPort.move(viewVel);
		m_stackedMoveChange = sf::Vector2f(m_stackedMoveChange.x + viewVel.x,
			m_stackedMoveChange.y + viewVel.y);
	}
}

void Display::MenuTab(Editor& editor)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			//sf::View view = editor.window.getView();
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
			if (ImGui::IsItemActive()) 
			{
				mainRect.scale(0.95, 0.95);
				mainViewPort.zoom(0.95);
			}
			ImGui::SameLine();
			if (ImGui::Button("ZOOMOUT"));
			if (ImGui::IsItemActive()) 
			{
				mainRect.scale(1.05, 1.05);
				mainViewPort.zoom(1.05);
			}
			mainViewPort.move(viewVel);
			m_stackedMoveChange = sf::Vector2f(	m_stackedMoveChange.x + viewVel.x,
												m_stackedMoveChange.y + viewVel.y);
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