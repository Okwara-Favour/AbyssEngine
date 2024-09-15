#pragma once
#include <vector>
#include <map>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"
#include <fstream>
#include <sstream>

class Assets
{
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, Animation> animations;
	std::map<std::string, sf::Sound> sounds;
	std::map<std::string, sf::Font> fonts;
	std::map<std::string, std::shared_ptr<sf::Shader>> shaders;
public:
	Assets();
	void addTexture  (const std::string& name, const std::string& path);
	void addAnimation(const std::string& name, const Animation  & anim);
	void addSound	 (const std::string& name, const std::string& path);
	void addFont	 (const std::string& name, const std::string& path);
	void addShader	 (const std::string& name, const std::string& path, const std::string& type = "fragment");

	const sf::Texture&	getTexture(const std::string& name);
	const Animation&	getAnimation(const std::string& name);
	const sf::Sound&	getSound(const std::string& name);
	const sf::Font&		getFont(const std::string& name);
	std::shared_ptr<sf::Shader>&	getShader(const std::string& name);

	const std::map<std::string, Animation>& getAnimations();
	const std::map<std::string, sf::Texture>& getTextures();

	void loadFromFile(const std::string& path);
};

