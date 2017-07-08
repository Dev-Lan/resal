#pragma once
#include <G3D/G3DALL.h>
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"
#include "Ray2D.hpp"

class Emitter {
public:
	Emitter(const shared_ptr< Dolly >& dolly=shared_ptr< Dolly >(), float wavelength=400, float intensity=1.0f, const Ray2D& ray=Ray2D(Point2(0,0), Vector2(1,0)));
	~Emitter();
	Emitter(const Emitter& other);
	Emitter& operator=(const Emitter& other);
	void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	Lasers::Laser laser;
	std::shared_ptr< Dolly > dolly;
private:
	Ray2D _initRay;
};