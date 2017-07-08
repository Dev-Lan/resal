#include "LaserScene.hpp"

LaserScene::LaserScene() {

}

LaserScene::~LaserScene() {
	for (unsigned int i=0; i < _interactors.size(); i++) {
		delete _interactors[i];
	}
}

LaserScene::LaserScene(const shared_ptr< Texture >& spectrumTex, const G3D::String& filename) {
	_spectrumTex = spectrumTex;
	_trackSlider.normal = Texture::fromFile(TEXTURE_DIR + "trackSlider.png");
	_trackSlider.onHover = Texture::fromFile(TEXTURE_DIR + "trackSlider-onHover.png");
	_trackSlider.onSelect = Texture::fromFile(TEXTURE_DIR + "trackSlider-onSelect.png");
	
	_transWidget.normal = Texture::fromFile(TEXTURE_DIR + "transWidget.png");
	_transWidget.onHover = Texture::fromFile(TEXTURE_DIR + "transWidget-onHover.png");
	_transWidget.onSelect = Texture::fromFile(TEXTURE_DIR + "transWidget-onSelect.png");
	
	_rotWidget.normal = Texture::fromFile(TEXTURE_DIR + "rotWidget.png");
	_rotWidget.onHover = Texture::fromFile(TEXTURE_DIR + "rotWidget-onHover.png");
	_rotWidget.onSelect = Texture::fromFile(TEXTURE_DIR + "rotWidget-onSelect.png");
	reloadFromXMLFile(filename);
}

void LaserScene::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	for (unsigned int i=0; i < _dollies.size(); i++) {
		_dollies[i]->onUserInput(ui, camera);
	}
	for (unsigned int i=0; i < _emitters.size(); i++) {
		_emitters[i].onUserInput(ui, camera);
	}
	for (unsigned int i=0; i < _interactors.size(); i++) {
		_interactors[i]->onUserInput(ui, camera);
	}
	for (unsigned int i=0; i < _receivers.size(); i++) {
		_receivers[i].onUserInput(ui, camera);
	}
	if (ui->keyPressed(GKey('r'))) {
		reloadFromXMLFile(_currentXMLFilename);
	}
}

void LaserScene::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	for (unsigned int i=0; i < _dollies.size(); i++) {
		_dollies[i]->onGraphics3D(rd, renderQuality, laserHeight);
	}
	for (unsigned int i=0; i < _emitters.size(); i++) {
		_emitters[i].onGraphics3D(rd, renderQuality, laserHeight);
	}
	for (unsigned int i=0; i < _interactors.size(); i++) {
		_interactors[i]->onGraphics3D(rd, renderQuality, laserHeight);
	}
	for (unsigned int i=0; i < _receivers.size(); i++) {
		_receivers[i].onGraphics3D(rd, renderQuality, laserHeight);
	}
}

Array< Lasers::LaserPoint > LaserScene::simulateLasers() {
	Array< Lasers::LaserPoint > laserPoints;
	std::queue< Lasers::Laser > laserQ;
	for (unsigned int i=0; i < _emitters.size(); i++) {
		laserQ.push(_emitters[i].laser);
	}
	for (unsigned int i=0; i < _receivers.size(); i++) {
		_receivers[i].resetPowerReq();
	}
	int laserNum = 0;
	const int lazyLimit = 160;
	while (!laserQ.empty()) {
		Lasers::Laser currentLaser = laserQ.front();
		laserQ.pop();
		laserNum++;

		float ttc = -1;
		bool hitsReceiver = false;
		int index = -1;
		for (unsigned int i=0; i < _receivers.size(); i++) {
			float t = _receivers[i].timeToCollision(currentLaser);
			// todo :: clean this when not tired
			if ( ttc < 0 || (t < ttc && t > 0) ) {
				ttc = t;
				hitsReceiver = true;
				index = i;
			}
		}
		for (unsigned int i=0; i < _interactors.size(); i++) {
			float t = _interactors[i]->timeToCollision(currentLaser);
			if (ttc < 0 || (t < ttc && t > 0) ) {
				ttc = t;
				hitsReceiver = false;
				index = i;
			}
		}
		// get laser points takes a laser and time and returns two points, if (ttc < 0) it assumes nothing is hit.
		std::pair< Lasers::LaserPoint, Lasers::LaserPoint> newPoints = Lasers::getLaserPoints(currentLaser, ttc, _spectrumTex);
		laserPoints.append(newPoints.first);
		laserPoints.append(newPoints.second);
		if (ttc > 0) {
			// if the laser hasn't lost power, either power receiever, or check for resulting lasers.
			if (newPoints.second.intensity > 0) {
				if (hitsReceiver) {
					_receivers[index].updatePowerReq(currentLaser.wavelength, newPoints.second.intensity);
				} else {
					Array< Lasers::Laser > resultingLasers = _interactors[index]->getResultingLasers(currentLaser);
					for (int i=0; i < resultingLasers.length(); i++) {
						laserQ.push(resultingLasers[i]);
					}
				}
			}
		}
		if (laserNum >= lazyLimit) {
			std::queue< Lasers::Laser > empty;
			std::swap( laserQ, empty );
			std::cout << "WARNING : Lasers exceeded" << lazyLimit << ", stopping simulation here." << std::endl; 
		}
	}
	
	_powered = true;
	for (unsigned int i=0; i < _receivers.size(); i++) {
		if (!_receivers[i].isPowered()) {
			_powered = false;
			break;
		}
	}
	if (_powered && !Dolly::anyDollySelected()) {
		Dolly::lockDollies();
		_levelSolved = true;
	}

	// std::cout << "laserNum : " << laserNum << std::endl;
	return laserPoints;
}

void LaserScene::reloadFromXMLFile(const G3D::String& filename) {
	_dollies.clear();
	_emitters.clear();
	_receivers.clear();
	for (unsigned int i=0; i < _interactors.size(); i++) {
		delete _interactors[i];
	}
	_interactors.clear();
	Dolly::unlockDollies();
	_levelSolved = false;
	loadSceneFromXMLFile(filename);

	_allLaserColor = Color3(0,0,0);
	Color3 c;
	for (unsigned int i=0; i < _receivers.size(); i++) {
		c = _receivers[i].colorReq();
		_allLaserColor += c;
	}
}


void LaserScene::loadSceneFromXMLFile(const G3D::String& filename) {
	_currentXMLFilename = filename;
	rapidxml::file<> xmlFile(filename.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(xmlFile.data());

	rapidxml::xml_node<>* sceneNode = doc.first_node("Scene"); 
	rapidxml::xml_node<>* dolliesNode = sceneNode->first_node("Dollies");
	for (rapidxml::xml_node<>* dollyNode = dolliesNode->first_node(); dollyNode; dollyNode = dollyNode->next_sibling()) {
		loadDollyNode(dollyNode);
	}

	rapidxml::xml_node<>* emittersNode = sceneNode->first_node("Emitters");
	for (rapidxml::xml_node<>* emitterNode = emittersNode->first_node(); emitterNode; emitterNode = emitterNode->next_sibling()) {
		loadEmitterNode(emitterNode);
	}
	
	rapidxml::xml_node<>* interactorsNode = sceneNode->first_node("Interactors");
	for (rapidxml::xml_node<>* interactorNode = interactorsNode->first_node(); interactorNode; interactorNode = interactorNode->next_sibling()) {
		if ( strncmp(interactorNode->name(), "ArcLens", 7) == 0 ) {
			loadArcLensNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "ArcMirror", 9) == 0 ) {
			loadArcMirrorNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "CircularLens", 12) == 0 ) {
			loadCircularLensNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "CircularMirror", 14) == 0 ) {
			loadCircularMirrorNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "FlatLens", 8) == 0 ) {
			loadFlatLensNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "FlatMirror", 10) == 0 ) {
			loadFlatMirrorNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "Splitter", 8) == 0 ) {
			loadSplitterNode(interactorNode);
		} else if ( strncmp(interactorNode->name(), "Wall", 4) == 0 ) {
			loadWallNode(interactorNode);
		}
	}
	
	rapidxml::xml_node<>* receiversNode = sceneNode->first_node("Receivers");
	for (rapidxml::xml_node<>* receiverNode = receiversNode->first_node(); receiverNode; receiverNode = receiverNode->next_sibling()) {
		loadReceiverNode(receiverNode);
	}
}

void LaserScene::loadDollyNode(rapidxml::xml_node<>* dollyNode) {
	// 	<dolly x1="" y1="" x2="" y2="" rotation="" trackSlider="" rotationMin="" rotationMax="" rotationLocked="" translationLocked=""/>
	float x1, x2, y1, y2, rotation, trackSlider, rotationMin, rotationMax;
	x1 = x2 = y1 = y2 = rotation = rotationMin = rotationMax = 0.0f;
	trackSlider = 0.5f;
	bool rotationLocked = false;
	bool translationLocked = false;
	for (rapidxml::xml_attribute<> *attr = dollyNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"x1",2) == 0 ){
			x1 = atof(attr->value());
		} else if ( strncmp(attr->name(),"x2",2) == 0 ) {
			x2 = atof(attr->value());
		} else if ( strncmp(attr->name(),"y1",2) == 0 ) {
			y1 = atof(attr->value());
		} else if ( strncmp(attr->name(),"y2",2) == 0 ) {
			y2 = atof(attr->value());
		} else if ( strncmp(attr->name(),"rotation",14) == 0 ) {
			rotation = atof(attr->value());
		} else if ( strncmp(attr->name(),"trackSlider",11) == 0 ) {
			trackSlider = atof(attr->value());
		} else if ( strncmp(attr->name(),"rotationMin",11) == 0 ) {
			rotationMin = atof(attr->value());
		} else if ( strncmp(attr->name(),"rotationMax",11) == 0 ) {
			rotationMax = atof(attr->value());
		} else if ( strncmp(attr->name(),"rotationLocked",14) == 0 ) {
			rotationLocked = true;
		} else if ( strncmp(attr->name(),"translationLocked",17) == 0 ) {
			translationLocked = true;
		}
	}
	LineSegment2D ls = LineSegment2D::fromTwoPoints(Point2(x1, y1), Vector2(x2, y2));
	shared_ptr< Dolly > newDolly = shared_ptr< Dolly >(new Dolly(_trackSlider, _transWidget, _rotWidget, ls, rotation, trackSlider, rotationMin, rotationMax, rotationLocked, translationLocked));
	_dollies.push_back(newDolly);
}

void LaserScene::loadEmitterNode(rapidxml::xml_node<>* emitterNode) {
	// <emitter dolly="0" wavelength="400" intensity="1" x="0" y="0" vx="1" vy="0"/>
	float wavelength, intensity, x, y, vx, vy;
	wavelength = 400;
	intensity = vy = 1;
	x = y = vx = 0;
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = emitterNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"x",2) == 0 ){
			x = atof(attr->value());
		} else if ( strncmp(attr->name(),"y",2) == 0 ) {
			y = atof(attr->value());
		} else if ( strncmp(attr->name(),"vx",2) == 0 ) {
			vx = atof(attr->value());
		} else if ( strncmp(attr->name(),"vy",2) == 0 ) {
			vy = atof(attr->value());
		} else if ( strncmp(attr->name(),"intensity",8) == 0 ) {
			intensity = atof(attr->value());
		} else if ( strncmp(attr->name(),"wavelength",11) == 0 ) {
			wavelength = atof(attr->value());
		} else if ( strncmp(attr->name(),"dolly",11) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Ray2D ray(Point2(x, y), Vector2(vx,vy).direction());
	Emitter newEmitter(_dollies[dollyIndex], wavelength, intensity, ray);
	_emitters.push_back(newEmitter);
}

void LaserScene::loadArcLensNode(rapidxml::xml_node<>* interactorNode) {
	// <ArcLens dolly="1" radius="5" angleWidth="3.14" x="0" y="0" vx="1" vy="0" isConcave=""/>
	float radius, angleWidth, x, y, vx, vy;
	radius = x = y = vx = 0;
	angleWidth = vy = 1;
	bool isConcave = false;
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"x",2) == 0 ){
			x = atof(attr->value());
		} else if ( strncmp(attr->name(),"y",2) == 0 ) {
			y = atof(attr->value());
		} else if ( strncmp(attr->name(),"vx",2) == 0 ) {
			vx = atof(attr->value());
		} else if ( strncmp(attr->name(),"vy",2) == 0 ) {
			vy = atof(attr->value());
		} else if ( strncmp(attr->name(),"radius",6) == 0 ) {
			radius = atof(attr->value());
		} else if ( strncmp(attr->name(),"angleWidth",10) == 0 ) {
			angleWidth = atof(attr->value());
		} else if ( strncmp(attr->name(),"isConcave",9) == 0 ) {
			isConcave = true;
		} else if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Ray2D ray(Point2(x, y), Vector2(vx,vy).direction());
	ArcLens* newArcLens = new ArcLens(radius, angleWidth, ray, _dollies[dollyIndex], isConcave);
	_interactors.push_back(newArcLens);
}

void LaserScene::loadArcMirrorNode(rapidxml::xml_node<>* interactorNode) {
	// <ArcMirror dolly="1" radius="5" angleWidth="1.78" x="0" y="0" vx="1" vy="0" />
	float radius, angleWidth, x, y, vx, vy;
	radius = x = y = vx = 0;
	angleWidth = vy = 1;
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"x",2) == 0 ){
			x = atof(attr->value());
		} else if ( strncmp(attr->name(),"y",2) == 0 ) {
			y = atof(attr->value());
		} else if ( strncmp(attr->name(),"vx",2) == 0 ) {
			vx = atof(attr->value());
		} else if ( strncmp(attr->name(),"vy",2) == 0 ) {
			vy = atof(attr->value());
		} else if ( strncmp(attr->name(),"radius",6) == 0 ) {
			radius = atof(attr->value());
		} else if ( strncmp(attr->name(),"angleWidth",10) == 0 ) {
			angleWidth = atof(attr->value());
		} else if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Ray2D ray(Point2(x, y), Vector2(vx,vy).direction());
	ArcMirror* newArcMirror = new ArcMirror(radius, angleWidth, ray, _dollies[dollyIndex]);
	_interactors.push_back(newArcMirror);
}

void LaserScene::loadCircularLensNode(rapidxml::xml_node<>* interactorNode) {
	// <CircularLens dolly="1" radius="3" />
	float radius;
	radius = 2.0;
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"radius",6) == 0 ){
			radius = atof(attr->value());
		} else if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	CircularLens* newCircularLens = new CircularLens(radius, _dollies[dollyIndex]);
	_interactors.push_back(newCircularLens);
}

void LaserScene::loadCircularMirrorNode(rapidxml::xml_node<>* interactorNode) {
	// <CircularMirror dolly="1" radius="2" />
	float radius = 2.0;
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"radius",6) == 0 ){
			radius = atof(attr->value());
		} else if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	CircularMirror* newCircularMirror = new CircularMirror(radius, _dollies[dollyIndex]);
	_interactors.push_back(newCircularMirror);
}

void LaserScene::loadFlatLensNode(rapidxml::xml_node<>* interactorNode) {
	// <FlatLens dolly="2">
	//	... LineSegments
	// </FlatLens>
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Array< LineSegment2D > lineSegments = loadLineSegments(interactorNode);
	FlatLens* newFlatLense = new FlatLens(lineSegments, _dollies[dollyIndex]);
	_interactors.push_back(newFlatLense);
}

void LaserScene::loadFlatMirrorNode(rapidxml::xml_node<>* interactorNode) {
	// <FlatMirror dolly="1">
	//	... LineSegments
	// </FlatMirror>
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Array< LineSegment2D > lineSegments = loadLineSegments(interactorNode);
	// actually create that bad boy
	FlatMirror* newFlatMirror = new FlatMirror(lineSegments, _dollies[dollyIndex]);
	_interactors.push_back(newFlatMirror);
}

void LaserScene::loadSplitterNode(rapidxml::xml_node<>* interactorNode) {
	// <Splitter dolly="0">
	//	... LineSegments
	// </Splitter>
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Array< LineSegment2D > lineSegments = loadLineSegments(interactorNode);
	// actually create that bad boy
	Splitter* newSplitter = new Splitter(lineSegments, _dollies[dollyIndex]);
	_interactors.push_back(newSplitter);
}

void LaserScene::loadWallNode(rapidxml::xml_node<>* interactorNode) {
	// <Wall dolly="3">
	//	... LineSegments
	// </Wall>
	int dollyIndex = 0;
	for (rapidxml::xml_attribute<> *attr = interactorNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Array< LineSegment2D > lineSegments = loadLineSegments(interactorNode);
	// actually create that bad boy
	Wall* newWall = new Wall(lineSegments, _dollies[dollyIndex]);
	_interactors.push_back(newWall);
}

Array< LineSegment2D > LaserScene::loadLineSegments(rapidxml::xml_node<>* interactorNode) {
	Array< LineSegment2D > lineSegments;
	for (rapidxml::xml_node<>* lineSegmentNode = interactorNode->first_node(); lineSegmentNode; lineSegmentNode = lineSegmentNode->next_sibling()) {
		float x1, x2, y1, y2;
		x1 = x2 = y1 = y2 = 0.0f;
		for (rapidxml::xml_attribute<> *attr = lineSegmentNode->first_attribute(); attr; attr = attr->next_attribute()) {
			if ( strncmp(attr->name(),"x1",2) == 0 ){
				x1 = atof(attr->value());
			} else if ( strncmp(attr->name(),"x2",2) == 0 ) {
				x2 = atof(attr->value());
			} else if ( strncmp(attr->name(),"y1",2) == 0 ) {
				y1 = atof(attr->value());
			} else if ( strncmp(attr->name(),"y2",2) == 0 ) {
				y2 = atof(attr->value());
			}
		}
		LineSegment2D ls = LineSegment2D::fromTwoPoints(Point2(x1,y1), Vector2(x2,y2));
		lineSegments.append(ls);
	}
	return lineSegments;
}


void LaserScene::loadReceiverNode(rapidxml::xml_node<>* receiverNode){
	// <Receiver dolly="3" radius="5">
	// 	<PowerReq wavelength="400" intensity="0.1"/>
	// </Receiver>
	int dollyIndex = 0;
	float radius = 2.0f;
	for (rapidxml::xml_attribute<> *attr = receiverNode->first_attribute(); attr; attr = attr->next_attribute()) {
		if ( strncmp(attr->name(),"radius",6) == 0 ){
			radius = atof(attr->value());
		} else if ( strncmp(attr->name(),"dolly",5) == 0 ) {
			dollyIndex = atoi(attr->value());
		}
	}
	Array< Receiver::powerReq > powerReqs;
	for (rapidxml::xml_node<>* powerReqNode = receiverNode->first_node(); powerReqNode; powerReqNode = powerReqNode->next_sibling()) {
		Receiver::powerReq powerReq;
		powerReq.wavelength = 400;
		powerReq.intensity = 0.0f;
		powerReq.isPowered = false;
		for (rapidxml::xml_attribute<> *attr = powerReqNode->first_attribute(); attr; attr = attr->next_attribute()) {
			if ( strncmp(attr->name(),"wavelength",10) == 0 ){
				powerReq.wavelength = atof(attr->value());
			} else if ( strncmp(attr->name(),"intensity",9) == 0 ) {
				powerReq.intensity = atof(attr->value());
			}
		}
		powerReqs.append(powerReq);
	}
	Receiver newReceiver(radius, powerReqs, _dollies[dollyIndex], _spectrumTex);
	_receivers.push_back(newReceiver);
}

void LaserScene::saveSceneToXMLFile(const G3D::String& filename) {
	// todo





















}