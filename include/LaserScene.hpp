#pragma once
#include <queue>
#include <vector>
#include <G3D/G3DAll.h>
#include "config.hpp"
#include "Emitter.hpp"
#include "Receiver.hpp"
#include "Interactor.hpp"
#include "IncludeInteractors.hpp"
#include "Laser.hpp"
#include "Dolly.hpp"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

class LaserScene {
public:
	LaserScene();
	~LaserScene();
	// todo rule of three
	// LaserScene(const shared_ptr< Texture >& spectrumTex);
	LaserScene(const shared_ptr <Texture >& spectrumTex, const G3D::String& filename);
	void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	Array< Lasers::LaserPoint > simulateLasers();

	inline bool LevelSolved() { return _levelSolved; }

	inline Color3 allLaserColor() { return _allLaserColor; }

	void reloadFromXMLFile(const G3D::String& filename);

private:

	void loadSceneFromXMLFile(const G3D::String& filename);
	void saveSceneToXMLFile(const G3D::String& filename);

	void loadDollyNode(rapidxml::xml_node<>* dollyNode);

	void loadEmitterNode(rapidxml::xml_node<>* emitterNode);

	// All dem interactor types...
	void loadArcLensNode(rapidxml::xml_node<>* interactorNode);
	void loadArcMirrorNode(rapidxml::xml_node<>* interactorNode);
	void loadCircularLensNode(rapidxml::xml_node<>* interactorNode);
	void loadCircularMirrorNode(rapidxml::xml_node<>* interactorNode);
	void loadFlatLensNode(rapidxml::xml_node<>* interactorNode);
	void loadFlatMirrorNode(rapidxml::xml_node<>* interactorNode);
	void loadSplitterNode(rapidxml::xml_node<>* interactorNode);
	void loadWallNode(rapidxml::xml_node<>* interactorNode);
	Array< LineSegment2D > loadLineSegments(rapidxml::xml_node<>* interactorNode);


	void loadReceiverNode(rapidxml::xml_node<>* receiverNode);

	std::vector< shared_ptr< Dolly > > _dollies;
	std::vector< Emitter > _emitters;
	std::vector< Receiver > _receivers;
	std::vector< Interactor* > _interactors;
	// int _maxLaserBounces;
	bool _powered;
	bool _levelSolved;
	Color3 _allLaserColor;
	shared_ptr< Texture > _spectrumTex;
	G3D::String _currentXMLFilename;
	// dolly textures
	Dolly::TextureSet _trackSlider;
	Dolly::TextureSet _transWidget;
	Dolly::TextureSet _rotWidget;
};