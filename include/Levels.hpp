#pragma once
#include <G3D/G3DALL.h>
#include "config.hpp"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

class Levels {
public:

	struct Level
	{
		G3D::String title;
		G3D::String xmlFilename;
		G3D::String thumbnailFilename;
		bool unlocked;
	};

	Levels(const G3D::String& filename);

	inline Level currentLevel() { return _levels[_currentLevel]; }

	Level nextLevel();

	Level previousLevel();

	Level getLevel(int index);

	void unlockLevel(int index);

private:
	void loadLevelsFromXMLFile(const G3D::String& filename);
  
  	int _currentLevel;
  	Array< Level > _levels;

};
