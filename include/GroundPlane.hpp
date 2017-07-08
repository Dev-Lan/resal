#pragma once
#include <G3D/G3DAll.h>
#include "config.hpp"
#include "Laser.hpp"
// CFrame::fromXYZYPRDegrees(  0.0f, -10.0f,  36.0f,   0.0f,  20.0f,   0.0f);
class GroundPlane {
public:
	GroundPlane(float width, float length, shared_ptr< Texture > groundTexture, shared_ptr< Texture > groundNrm) : _groundWidth(width), _groundLength(length)
	{
		_groundTex = groundTexture;
		_groundNrm = groundNrm;
		initGeometry();
		// TEMP FOR DEBUGGING
		d_useAmbient = true;
		d_useDiffuse = true;
		d_useSpecular = true;
	}
	void onGraphics3D(RenderDevice* rd, const Array< Lasers::LaserPoint > laserPoints, RENDER_QUALITY renderQuality, float laserHeight);

	// TEMP FOR DEBUGGING
	Vector3 d_lightPos;
	bool d_useNormalMap;
	bool d_useTextureMap;
	bool d_useAmbient;
	bool d_useDiffuse;
	bool d_useSpecular;

	Color3 d_lightColor;
	float d_alpha;


private:
	shared_ptr< Texture > _groundTex;
	shared_ptr< Texture > _groundNrm;
	float _groundWidth;
	float _groundLength;

	// for sending to the graphics card
	shared_ptr< VertexBuffer > _vbuffer_ground;
	AttributeArray _gpuVerts_ground;
	AttributeArray _gpuNorms_ground;
	AttributeArray _gpuTexCoords_ground;
	IndexStream _gpuIndices_ground;
	
	void initGeometry();
	void onGraphics3D_HIGH(RenderDevice* rd, const Array< Lasers::LaserPoint > laserPoints, float laserHeight);
	void onGraphics3D_DEBUG(RenderDevice* rd);
};
