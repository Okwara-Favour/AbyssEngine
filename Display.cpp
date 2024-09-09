#include "Display.h"
#include "Editor.h"
void Display::Init(Editor& editor)
{
	displayTexture.create(800, 600);
}
void Display::Update(Editor& editor)
{
	ImGui::Begin("Display");
	for (auto& e : editor.entityManager.getEntities())
	{
		displayTexture.clear(sf::Color(100,100,100));
		if (e->hasComponent<CShape>())
		{
			displayTexture.draw(e->getComponent<CShape>().rectangle);
		}
		displayTexture.display();

		ImGui::Image((void*)(intptr_t)displayTexture.getTexture().getNativeHandle(),
			ImVec2(displayTexture.getSize().x, displayTexture.getSize().y),
			ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}