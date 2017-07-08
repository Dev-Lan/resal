#pragma once
#include <G3D/G3DALL.h>
#include "Interactor.hpp"
#include "Ray2D.hpp"
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"

class CircularMirror : public Interactor{
public:
	CircularMirror();
	CircularMirror(float radius, const shared_ptr< Dolly >& dolly=shared_ptr<Dolly>());
	virtual ~CircularMirror();
	CircularMirror(const CircularMirror& other);
	CircularMirror& operator= (const CircularMirror& other);

	virtual void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	virtual void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	virtual float timeToCollision(const Lasers::Laser& laserIn);
	virtual Array< Lasers::Laser > getResultingLasers(const Lasers::Laser& laserIn);
	// virtual Dolly& getDolly();
private:
	shared_ptr< Dolly > _dolly;
	float _radius;
};