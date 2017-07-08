#include "GroundPlane.hpp"
//  +X -> right
//  +Y -> back/up
//  +Z -> out of screen / up

void GroundPlane::initGeometry() {
	// WIDTH  -> X-axis
	// LENGTH -> Y-axis
	Array< Vector3 > cpuVerts;
	Array< Vector3 > cpuNorms;
	Array< Vector2 > cpuTexCoords;
	Array< int > cpuIndices;

// +       x0y1 +-------------+ x1y1
// |            |             |
// |            |             |
// |            |             |
// |            |             |
// |            |             |
// +       x0y0 +-------------+ x1y0
	// verts
	Vector3 x0y0(- _groundWidth/2.0f, - _groundLength/2.0f, 0);
	Vector3 x1y0(  _groundWidth/2.0f, - _groundLength/2.0f, 0);
	Vector3 x0y1(- _groundWidth/2.0f,   _groundLength/2.0f, 0);
	Vector3 x1y1(  _groundWidth/2.0f,   _groundLength/2.0f, 0);
	// norms
	Vector3 groundNorm(0,0,1);

	cpuVerts.append(x0y0, x1y0, x0y1, x1y1);

	cpuNorms.append(groundNorm,groundNorm,groundNorm,groundNorm);
	// you have to invert the Y values.
	cpuTexCoords.append(Vector2(0,1));
	cpuTexCoords.append(Vector2(1,1));
	cpuTexCoords.append(Vector2(0,0));
	cpuTexCoords.append(Vector2(1,0));

	cpuIndices.append(0,1,2);
	cpuIndices.append(2,1,3);

	_vbuffer_ground = VertexBuffer::create( sizeof(Vector3) * cpuVerts.length()
										 + sizeof(Vector3) * cpuNorms.length()
										 + sizeof(Vector3) * cpuIndices.length()
										 + sizeof(Vector3) * cpuTexCoords.length(),
										 VertexBuffer::WRITE_ONCE);
	_gpuVerts_ground = AttributeArray(cpuVerts, _vbuffer_ground);
	_gpuNorms_ground = AttributeArray(cpuNorms, _vbuffer_ground);
	_gpuTexCoords_ground = AttributeArray(cpuTexCoords, _vbuffer_ground);
	_gpuIndices_ground = IndexStream(cpuIndices, _vbuffer_ground);

	// d_lightPos = Vector3(0,0,.7);
	d_useNormalMap = true;
	d_useTextureMap = true;
	d_alpha = 35.0f;
}

void GroundPlane::onGraphics3D(RenderDevice* rd, const Array< Lasers::LaserPoint > laserPoints, RENDER_QUALITY renderQuality, float laserHeight) {
	if (renderQuality == HIGH_QUALITY) {
		onGraphics3D_HIGH(rd, laserPoints, laserHeight);
	} else if (renderQuality == DEBUG_QUALITY) {
		onGraphics3D_DEBUG(rd);
	}
}

void GroundPlane::onGraphics3D_HIGH(RenderDevice* rd, const Array< Lasers::LaserPoint > laserPoints, float laserHeight) {
	Args args;
	// for debugging/testing
	args.setUniform("useNormalMap", d_useNormalMap);
	args.setUniform("useTextureMap", d_useTextureMap);
	args.setUniform("useAmbient", d_useAmbient);
	args.setUniform("useDiffuse", d_useDiffuse);
	args.setUniform("useSpecular", d_useSpecular);
	// end debugging code

	args.setUniform("tex", _groundTex, Sampler::video());
	args.setUniform("nrm", _groundNrm, Sampler::video());
	args.setUniform("K_A", Color3(0.18, 0.18, 0.18));
	args.setUniform("K_D", Color3(0.75, 0.75, 0.75));
	args.setUniform("K_S", Color3(0.95, 0.95, 0.95));

	args.setUniform("alpha", d_alpha);

	args.setMacro("NUM_LASER_POINTS", laserPoints.length());
	args.setMacro("LASER_HEIGHT", laserHeight);

	args.setUniform("numberOfLaserSegments", laserPoints.length() / 2);
	for (int i=0; i < laserPoints.length(); i++) {
		args.setArrayUniform("laserLightsPos", i, laserPoints[i].point);
		args.setArrayUniform("laserLightsCol", i, laserPoints[i].color);
		args.setArrayUniform("laserLightsIntensity", i, laserPoints[i].intensity);
	}
		
	Point3 camPosition = rd->cameraToWorldMatrix().translation;
	args.setUniform("camPos", camPosition);


	args.setAttributeArray("vert", _gpuVerts_ground);
	args.setAttributeArray("norm", _gpuNorms_ground);
	args.setAttributeArray("texCoord", _gpuTexCoords_ground);
	args.setIndexStream(_gpuIndices_ground);
	args.setPrimitiveType(PrimitiveType::TRIANGLES);
	const G3D::String shaderPattern = SHADER_DIR + "groundPlane.*";
	LAUNCH_SHADER(shaderPattern, args);
}

void GroundPlane::onGraphics3D_DEBUG(RenderDevice* rd) {
	Args args;
	args.setAttributeArray("vert", _gpuVerts_ground);
	args.setIndexStream(_gpuIndices_ground);
	args.setPrimitiveType(PrimitiveType::TRIANGLES);
	const G3D::String shaderPattern = SHADER_DIR + "debug.*";
	LAUNCH_SHADER(shaderPattern, args);
}
