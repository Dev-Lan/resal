#pragma once
#include <G3D/G3DALL.h>
#include "Interactor.hpp"
#include "Ray2D.hpp"
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"

class Splitter : public Interactor {
public:
	Splitter();
	Splitter(const Array< LineSegment2D > lineSegments, const std::shared_ptr< Dolly >& dolly=std::shared_ptr< Dolly >());
	virtual ~Splitter();
	Splitter(const Splitter& other);
	Splitter& operator= (const Splitter& other);

	virtual void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	virtual void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	virtual float timeToCollision(const Lasers::Laser& laserIn);
	virtual Array< Lasers::Laser > getResultingLasers(const Lasers::Laser& laserIn);
	// virtual Dolly& getDolly();
private:
	std::shared_ptr< Dolly > _dolly;
	Array< LineSegment2D > _lineSegments;
	Array< LineSegment2D > _initLineSegments;
};