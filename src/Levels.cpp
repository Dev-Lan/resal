#include "Levels.hpp"

Levels::Levels(const G3D::String& filename) {
	_currentLevel = 0;
	loadLevelsFromXMLFile(filename);
}

Levels::Level Levels::previousLevel() {
	_currentLevel = std::max(_currentLevel-1, 0);
	return currentLevel();
}

Levels::Level Levels::nextLevel() {
	_currentLevel = std::min(_currentLevel+1, _levels.length()-1);
	return currentLevel();
}

Levels::Level Levels::getLevel(int index) {
	_currentLevel = index;
	return currentLevel();
}

void Levels::unlockLevel(int index) {
	// todo
}

void Levels::loadLevelsFromXMLFile(const G3D::String& filename) {
	rapidxml::file<> xmlFile(filename.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

		G3D::String title;
		G3D::String xmlFilename;
		G3D::String thumbnailFilename;
		bool unlocked;

	rapidxml::xml_node<>* levelsNode = doc.first_node("Levels"); 
	for (rapidxml::xml_node<>* levelNode = levelsNode->first_node(); levelNode; levelNode = levelNode->next_sibling()) {
		Levels::Level newLevel;
		newLevel.unlocked = false;
		for (rapidxml::xml_attribute<> *attr = levelNode->first_attribute(); attr; attr = attr->next_attribute()) {
			if ( strncmp(attr->name(),"title",6) == 0 ) {
				newLevel.title = attr->value();
			} else if ( strncmp(attr->name(),"xmlFilename",11) == 0 ) {
				newLevel.xmlFilename = attr->value();
			} else if ( strncmp(attr->name(),"thumbnailFilename",17) == 0 ) {
				newLevel.thumbnailFilename = attr->value();
			} else if ( strncmp(attr->name(),"unlocked",8) == 0 ) {
				newLevel.unlocked = true;
			}
		}
		_levels.append(newLevel);
	}
}
