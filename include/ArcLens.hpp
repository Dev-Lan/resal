#pragma once
#include <math.h>
#include <G3D/G3DALL.h>
#include "Interactor.hpp"
#include "Ray2D.hpp"
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"

class ArcLens : public Interactor{
public:
	ArcLens();
	ArcLens(float radius, float angleWidth = pi(), const Ray2D& offset = Ray2D(Point2(0,0), Vector2(1,0)), const shared_ptr< Dolly>& dolly=shared_ptr<Dolly>(), bool isConcave = true);
	virtual ~ArcLens();
	ArcLens(const ArcLens& other);
	ArcLens& operator= (const ArcLens& other);

	virtual void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	virtual void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	virtual float timeToCollision(const Lasers::Laser& laserIn);
	virtual Array< Lasers::Laser > getResultingLasers(const Lasers::Laser& laserIn);
	// virtual Dolly& getDolly();
private:
	shared_ptr< Dolly > _dolly;
	float _radius;
	Ray2D _offset;
	float _angleWidth;
	bool _isConcave;
};