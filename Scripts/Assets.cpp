#include "Assets.h"

Assets::Assets()
{}
void Assets::addTexture(const std::string& name, const std::string& path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cerr << "Error: An error occurred in addTexture" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    textures[name] = texture;
}
void Assets::addAnimation(const std::string& name, const Animation& anim)
{
    animations[name] = anim;
}
void Assets::addSound(const std::string& name, const std::string& path)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path))
    {
        std::cerr << "Error: An error occurred in addSound" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    sf::Sound sound;
    sound.setBuffer(buffer);
    sounds[name] = sound;
}
void Assets::addFont(const std::string& name, const std::string& path)
{
    sf::Font font;
    if (!font.loadFromFile(path)) {
        std::cerr << "Error: An error occurred in addFont" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    fonts[name] = font;
}

void Assets::addShader(const std::string& name, const std::string& path, const std::string& type)
{
	std::shared_ptr<sf::Shader> shader = std::make_shared<sf::Shader>();
	if (type == "fragment") {
		if (!shader->loadFromFile(path, sf::Shader::Fragment)) {
			std::cerr << "Error: An error occurred in addShader" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	if (type == "vertex") {
		if (!shader->loadFromFile(path, sf::Shader::Vertex)) {
			std::cerr << "Error: An error occurred in addShader" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	shaders[name] = shader;
}

const sf::Texture& Assets::getTexture(const std::string& name)
{
    return textures[name];
}
const Animation& Assets::getAnimation(const std::string& name)
{
    return animations[name];
}
const sf::Sound& Assets::getSound(const std::string& name)
{
    return sounds[name];
}
const sf::Font& Assets::getFont(const std::string& name)
{
    return fonts[name];
}

std::shared_ptr<sf::Shader>& Assets::getShader(const std::string& name)
{
	return shaders[name];
}

const std::map<std::string, Animation>& Assets::getAnimations()
{
	return animations;
}

const std::map<std::string, sf::Texture>& Assets::getTextures()
{
	return textures;
}

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string name;
		iss >> name;

		if (name == "Texture")
		{
			std::string textureName, texturePath;
			iss >> textureName >> texturePath;
			addTexture(textureName, texturePath);
		}
		if (name == "Animation")
		{
			std::string AnimName, textureName;
			size_t AnimframeCount, AnimFrameSpeed;
			iss >> AnimName >> textureName >> AnimframeCount >> AnimFrameSpeed;
			Animation anim(AnimName, textureName, getTexture(textureName), AnimframeCount, AnimFrameSpeed);
			addAnimation(AnimName, anim);
		}
		if (name == "Font")
		{
			std::string fontName, fontPath;
			iss >> fontName >> fontPath;
			addFont(fontName, fontPath);
		}

		if (name == "Shader")
		{
			std::string shaderName, shaderPath, shaderType;
			iss >> shaderName >> shaderPath >> shaderType; 
			addShader(shaderName, shaderPath, shaderType);
		}
	}
	file.close();
}