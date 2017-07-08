#pragma once
#include <G3D/G3DAll.h>
#include "config.hpp"
#include "GroundPlane.hpp"
#include "LaserScene.hpp"
#include "Levels.hpp"

class LaserEngine {
public:
	LaserEngine();
	LaserEngine(float width, float height, shared_ptr< Texture > groundTexture, shared_ptr< Texture > groundNrm, Levels::Level level, RenderDevice* rd);
	~LaserEngine();
	LaserEngine(const LaserEngine& other);
	LaserEngine& operator= (const LaserEngine& other);
	void onGraphics3D(RenderDevice* rd);
	void onGraphics2D(RenderDevice* rd);
	void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	void onSimulation(float time);
	void loadLevel(Levels::Level level);
private:
	GroundPlane* _groundPlane;
	LaserScene* _laserScene;

	shared_ptr< Texture > _spectrumTex;
	RENDER_QUALITY _renderQuality;
	float _laserHeight;

	// for rendering GUI elements
	shared_ptr< Texture > _levelSolvedOverlay;
	AttributeArray _gpuVerts;
	AttributeArray _gpuTexCoords;
	IndexStream _gpuIndices;
	shared_ptr< VertexBuffer > _vbuf;

	// for debugging
	shared_ptr< Texture > d_rainbowTex;
	int d_rainbowIndex;
	float d_timeElapsedSinceLightColorChange;
	void renderLaserBeams(RenderDevice* rd, const Array< Lasers::LaserPoint >& laserPoints);
	void initGraphicsCard(RenderDevice* rd);
};