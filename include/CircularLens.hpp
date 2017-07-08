#pragma once
#include <G3D/G3DALL.h>
#include "Interactor.hpp"
#include "Ray2D.hpp"
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"

class CircularLens : public Interactor{
public:
	CircularLens();
	CircularLens(float radius, const std::shared_ptr< Dolly >& dolly=std::shared_ptr< Dolly >());
	virtual ~CircularLens();
	CircularLens(const CircularLens& other);
	CircularLens& operator= (const CircularLens& other);

	virtual void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	virtual void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	virtual float timeToCollision(const Lasers::Laser& laserIn);
	virtual Array< Lasers::Laser > getResultingLasers(const Lasers::Laser& laserIn);
	// virtual Dolly& getDolly();
private:
	std::shared_ptr< Dolly > _dolly;
	float _radius;
};