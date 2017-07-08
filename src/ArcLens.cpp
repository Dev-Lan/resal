#include "ArcLens.hpp"

ArcLens::ArcLens() { }

ArcLens::ArcLens(float radius, float angleWidth, const Ray2D& offset, const shared_ptr< Dolly>& dolly, bool isConcave) : _radius(radius), _angleWidth(angleWidth), _offset(offset), _isConcave(isConcave) {
	_dolly = dolly;
}

ArcLens::~ArcLens() { }

// todo update 
ArcLens::ArcLens(const ArcLens& other) : _dolly(other._dolly), _radius(other._radius), _offset(other._offset), _angleWidth(other._angleWidth), _isConcave(other._isConcave) { }

// todo update 
ArcLens& ArcLens::operator=(const ArcLens& other) {
	_dolly = other._dolly;
	_radius = other._radius;
	_offset = other._offset;
	_angleWidth = other._angleWidth;
	_isConcave = other._isConcave;
	return *this;
}


void ArcLens::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	// update angles
}

void ArcLens::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	renderQuality = DEBUG_QUALITY; // TODO - remove after impementing high quality
	if (renderQuality == DEBUG_QUALITY) {
		const int numSegs = (int)_radius + 5;
		// float tau = 2*pi();
		float dollyOffset = -_dolly->getRotation() + halfPi();
		float mirrorOffset = atan(-_offset.direction.y / _offset.direction.x);
		// mirrorOffset -= halfPi();
		if (_offset.direction.x < 0) {
			mirrorOffset += pi();
		}
		else if (_offset.direction.y < 0) {
			mirrorOffset += twoPi();
		}
		float minAngle = mirrorOffset - _angleWidth / 2.f;
		float maxAngle = mirrorOffset + _angleWidth / 2.f;
		Vector2 center = _dolly->getCenterPoint() + _dolly->getRotMat() * Vector2(_offset.point);
		for (int i=0; i < numSegs; i++) {
			float percent = float(i) / (float) numSegs;
			float x = _radius * sin(dollyOffset + minAngle + percent * (maxAngle - minAngle)) + center.x;
			float y = _radius * cos(dollyOffset + minAngle + percent * (maxAngle - minAngle)) + center.y;
			Vector3 p1(x,y,laserHeight);
			percent = float(i+1) / (float) numSegs;
			x = _radius * sin(dollyOffset + minAngle + percent * (maxAngle - minAngle)) + center.x;
			y = _radius * cos(dollyOffset + minAngle + percent * (maxAngle - minAngle)) + center.y;
			Vector3 p2(x,y,laserHeight);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			Draw::lineSegment(ls, rd, Color4(1,1,1,.5));
		}	
	} else if( renderQuality == HIGH_QUALITY) {
		// todo
	}
}

float ArcLens::timeToCollision(const Lasers::Laser& laserIn) {
	Point2 center = _dolly->getCenterPoint() + _dolly->getRotMat() * Vector2(_offset.point);
	float t = laserIn.ray.timeToIntersection(center, _radius, _angleWidth, _dolly->getRotMat() * _offset.direction );
	if (t > 0) {
		return t;
	} else {
		return -1.0f;
	}
}

Array< Lasers::Laser > ArcLens::getResultingLasers(const Lasers::Laser& laserIn) {
	// refind first mirror side hit, could keep track of this in last call, but this is safer.
	Point2 center = _dolly->getCenterPoint() + _dolly->getRotMat() * Vector2(_offset.point);
	float ttc = laserIn.ray.timeToIntersection(center, _radius, _angleWidth, _dolly->getRotMat() * _offset.direction );

	Vector2 v = laserIn.ray.point - center;
	Vector2 normal;
	if (_isConcave) {
		normal = (center - laserIn.ray.getPointFromTime(ttc)).direction();
	} else {
		normal = (laserIn.ray.getPointFromTime(ttc) - center).direction();
	}

	Array< Lasers::Laser > resultList;


	Point2 pos = laserIn.ray.getPointFromTime(ttc);
	Lasers::Laser reflectedLaser;
	reflectedLaser.ray = laserIn.ray.getRefractedRay(pos, normal, 1.0f, Lasers::getIndexOfRefractionFromWavelength(laserIn.wavelength));

	reflectedLaser.intensity = Lasers::intensityDecline(laserIn.intensity, ttc);
	reflectedLaser.wavelength = laserIn.wavelength;
	reflectedLaser.ray.point = reflectedLaser.ray.getPointFromTime(RAY_NEAR_EPS);
	resultList.append(reflectedLaser);

	return resultList;
}
