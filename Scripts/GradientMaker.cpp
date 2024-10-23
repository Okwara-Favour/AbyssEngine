#include <iostream>
#include "GradientMaker.h"

GradientMaker::GradientMaker(const sf::Vector2f& size, std::shared_ptr<sf::Shader>& shader, const std::string& shaderName)
	:m_size(size), m_shader(shader), m_shaderName(shaderName)
{}
void GradientMaker::makeGradient(const std::string& gradientName)
{
	if (m_shaderName == "SimpleGradient")
	{
		m_shader->setUniform("u_resolution", simpleGradient.u_resolution);
		m_shader->setUniform("windowSize", simpleGradient.windowSize);
		m_shader->setUniform("lightPosition", simpleGradient.lightPosition);
		m_shader->setUniform("lightcolorstart", simpleGradient.lightcolorstart);
		m_shader->setUniform("lightcolorend", simpleGradient.lightcolorend);
		m_shader->setUniform("lightRadius", simpleGradient.lightRadius);
	}

	sf::RenderTexture renderTexture;
	renderTexture.create(m_size.x, m_size.y);

    renderTexture.clear(sf::Color::Transparent);
    // Draw a full-screen quad using the shader
    sf::RectangleShape fullscreenQuad(sf::Vector2f(m_size.x, m_size.y));
    renderTexture.draw(fullscreenQuad, m_shader.get());
    renderTexture.display();

    // Retrieve the texture from the render texture
    const sf::Texture& texture = renderTexture.getTexture();
    sf::Image image = texture.copyToImage();

    std::string fileName = gradientName + ".png";
    // Save the image to a file
    if (!image.saveToFile(fileName))
    {
        std::cerr << "Failed to save image to file" << std::endl;
    }
}