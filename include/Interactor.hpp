#pragma once
#include <G3D/G3DALL.h>
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"
// virtual class

class Interactor {
public:
	Interactor();
	virtual ~Interactor();
	// Interactor(const Interactor& other);
	// Interactor& operator=(const Interactor& other);

	virtual void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) = 0;
	virtual void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) = 0;
	virtual float timeToCollision(const Lasers::Laser& laserIn) = 0;
	virtual Array< Lasers::Laser > getResultingLasers(const Lasers::Laser& laserIn) = 0;
	// virtual Dolly& getDolly() = 0;

};