#pragma once
#include <G3D/G3DALL.h>
#include "Interactor.hpp"
#include "Ray2D.hpp"
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"

class FlatMirror : public Interactor{
public:
	FlatMirror();
	FlatMirror(const Array< LineSegment2D > lineSegments, const std::shared_ptr< Dolly >& dolly=std::shared_ptr< Dolly >(), const Array< Vector2 >& normals=Array< Vector2 >());
	virtual ~FlatMirror();
	FlatMirror(const FlatMirror& other);
	FlatMirror& operator= (const FlatMirror& other);

	virtual void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	virtual void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	virtual float timeToCollision(const Lasers::Laser& laserIn);
	virtual Array< Lasers::Laser > getResultingLasers(const Lasers::Laser& laserIn);
	// virtual Dolly& getDolly();
private:
	std::shared_ptr< Dolly > _dolly;
	Array< LineSegment2D > _lineSegments;
	Array< LineSegment2D > _initLineSegments;
	Array< Vector2 > _normals;

};