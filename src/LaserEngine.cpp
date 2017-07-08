#include "LaserEngine.hpp"

LaserEngine::LaserEngine() {

}

LaserEngine::LaserEngine(float width, float height, shared_ptr< Texture > groundTexture, shared_ptr< Texture > groundNrm, Levels::Level level, RenderDevice* rd) {
	_spectrumTex = Texture::fromFile(SPECTRUM_FILENAME);
	_levelSolvedOverlay = Texture::fromFile(TEXTURE_DIR + "levelSolvedOverlay.png");
	_groundPlane = new GroundPlane(width, height, groundTexture, groundNrm);
	if (DEBUG_ENABLED) {
		d_rainbowTex = Texture::fromFile(LIGHT_COLORS_FILENAME);
		d_rainbowIndex = 0;
		d_timeElapsedSinceLightColorChange = 0;
	}
	_laserScene = new LaserScene(_spectrumTex, SCENE_DIR + level.xmlFilename + ".xml");
	_renderQuality = HIGH_QUALITY;
	_laserHeight = 0.6f;
	initGraphicsCard(rd);
}

LaserEngine::~LaserEngine() {
	delete _groundPlane;
	delete _laserScene;
}

// LaserEngine::LaserEngine(const LaserEngine& other) {

// }

// LaserEngine::LaserEngine& operator= (const LaserEngine& other) {

// }

void LaserEngine::initGraphicsCard(RenderDevice* rd)
{
	/*
	Index ordering for these windows
			0           1
			+-----------+
			|           |
			|           |
			+-----------+
			3           2
	*/
	Array< Point2 > cpuVerts;
	cpuVerts.append(Point2(-1,-1), Point2(1,-1), Point2(1,1), Point2(-1,1));

	Array< Point2 > cpuTexCoords;
	cpuTexCoords.append(Point2(0,0), Point2(1,0), Point2(1,1), Point2(0,1));

	Array< int > cpuIndices;
	cpuIndices.append(0,1,2);
	cpuIndices.append(2,3,0);

	// to the GPU with you
	_vbuf = VertexBuffer::create(
							sizeof(Point2) * cpuVerts.size() +
							sizeof(Point2) * cpuTexCoords.size() + 
							sizeof(int)    * cpuIndices.size(),
							VertexBuffer::WRITE_ONCE );

	_gpuVerts     = AttributeArray(cpuVerts, _vbuf);
	_gpuTexCoords = AttributeArray(cpuTexCoords, _vbuf);
	_gpuIndices   = IndexStream(cpuIndices, _vbuf);
}


void LaserEngine::onGraphics3D(RenderDevice* rd) {
	Array< Lasers::LaserPoint > laserPoints = _laserScene->simulateLasers();

	_groundPlane->onGraphics3D(rd, laserPoints, _renderQuality, _laserHeight);
	
	_laserScene->onGraphics3D(rd, _renderQuality, _laserHeight);
	renderLaserBeams(rd, laserPoints);


}

void LaserEngine::onGraphics2D(RenderDevice* rd) {
	if (_laserScene->LevelSolved()) {
		rd->pushState();
		{
			// rd->setBlendFunc(Framebuffer::COLOR0, RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA, RenderDevice::BLENDEQ_ADD);
			rd->setBlendFunc(Framebuffer::COLOR0, RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA, RenderDevice::BLENDEQ_ADD);
			const G3D::String shaderPattern = SHADER_DIR + "2DTextured.*";
			Args args;
			args.setUniform("tex", _levelSolvedOverlay, Sampler::video());
			args.setUniform("spectrumTex", _spectrumTex, Sampler::video());
		
			args.setUniform("allLaserColor", _laserScene->allLaserColor());

			args.setAttributeArray("vert", _gpuVerts);
			args.setAttributeArray("texCoord", _gpuTexCoords);
			args.setIndexStream(_gpuIndices);
			args.setPrimitiveType(PrimitiveType::TRIANGLES);
			LAUNCH_SHADER(shaderPattern, args);
		}
		rd->popState();
	}
}


void LaserEngine::onUserInput(UserInput* ui,  const shared_ptr< Camera >& camera) {
	if (DEBUG_ENABLED) {

		if (ui->keyPressed(GKey('n'))) {
			_groundPlane->d_useNormalMap = !_groundPlane->d_useNormalMap;
		}
		if (ui->keyPressed(GKey('t'))) {
			_groundPlane->d_useTextureMap = !_groundPlane->d_useTextureMap;
		}

		if (ui->keyDown(GKey('='))) {
			_groundPlane->d_alpha *= 1.025f;
			std::cout << "ALPHA : " << _groundPlane->d_alpha << std::endl;
		}
		if (ui->keyDown(GKey('-'))) {
			_groundPlane->d_alpha /= 1.025f;
			std::cout << "ALPHA : " << _groundPlane->d_alpha << std::endl;
		}
		if (ui->keyPressed(GKey('1'))) {
			_groundPlane->d_useAmbient = !_groundPlane->d_useAmbient;
			std::cout << "ambient set to : " << _groundPlane->d_useAmbient << std::endl;
		}
		if (ui->keyPressed(GKey('2'))) {
			_groundPlane->d_useDiffuse = !_groundPlane->d_useDiffuse;
			std::cout << "diffuse set to : " << _groundPlane->d_useDiffuse << std::endl;
		}
		if (ui->keyPressed(GKey('3'))) {
			_groundPlane->d_useSpecular = !_groundPlane->d_useSpecular;
			std::cout << "specular set to : " << _groundPlane->d_useSpecular << std::endl;
		}
		if (ui->keyPressed(GKey(' '))) {
			if (_renderQuality == HIGH_QUALITY) {
				_renderQuality = DEBUG_QUALITY;
			} else {
				_renderQuality = HIGH_QUALITY;
			}
		}
	}
	_laserScene->onUserInput(ui, camera);

}

void LaserEngine::onSimulation(float time) {
	if (DEBUG_ENABLED) {
		d_timeElapsedSinceLightColorChange += time;
		if (d_timeElapsedSinceLightColorChange > .3) {
			d_timeElapsedSinceLightColorChange = 0;
			d_rainbowIndex++;
		}
	}
}

void LaserEngine::loadLevel(Levels::Level level) {
	_laserScene->reloadFromXMLFile(SCENE_DIR + level.xmlFilename + ".xml");
}

void LaserEngine::renderLaserBeams(RenderDevice* rd, const Array< Lasers::LaserPoint>& laserPoints) {
	// todo check _renderQuality
	if (_renderQuality == DEBUG_QUALITY) {
		for (int i=0; i < laserPoints.length()-1; i+=2) {
			Point3 p1 = Point3(laserPoints[i].point.x, laserPoints[i].point.y, _laserHeight);
			Point3 p2 = Point3(laserPoints[i+1].point.x, laserPoints[i+1].point.y, _laserHeight);
			Color4 color = Color4(laserPoints[i].color, laserPoints[i].intensity / 2.0f);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			Draw::lineSegment(ls ,rd, color);
		}
	} else if (_renderQuality == HIGH_QUALITY) {
		// todo
	}

}
