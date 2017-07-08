#include "FlatLens.hpp"

FlatLens::FlatLens() { }

FlatLens::FlatLens(const Array< LineSegment2D > lineSegments, const std::shared_ptr< Dolly >& dolly) : _initLineSegments(lineSegments) {
	_dolly = dolly;
	for (int i=0; i < lineSegments.length(); i++) {
		Matrix2 rotMat = dolly->getRotMat();
		Point2 p1 = rotMat * lineSegments[i].point(0) + Vector2(_dolly->getCenterPoint());
		Point2 p2 = rotMat * lineSegments[i].point(1) + Vector2(_dolly->getCenterPoint());
		_lineSegments.append(LineSegment2D::fromTwoPoints(p1,p2));
	}
}

FlatLens::~FlatLens() { }

// todo update for lists
FlatLens::FlatLens(const FlatLens& other) : _dolly(other._dolly) { }

// todo update for lists
FlatLens& FlatLens::operator=(const FlatLens& other) {
	_dolly = other._dolly;
	return *this;
}


void FlatLens::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	for (int i=0; i < _initLineSegments.length(); i++) {
		Matrix2 rotMat = _dolly->getRotMat();
		Point2 p1 = rotMat * _initLineSegments[i].point(0) + Vector2(_dolly->getCenterPoint());
		Point2 p2 = rotMat * _initLineSegments[i].point(1) + Vector2(_dolly->getCenterPoint());
		_lineSegments[i] = LineSegment2D::fromTwoPoints(p1,p2);
	}
}

void FlatLens::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	renderQuality = DEBUG_QUALITY; // TODO - remove after impementing high quality
	if (renderQuality == DEBUG_QUALITY) {
		for (int i=0; i < _lineSegments.length(); i++) {
			Point3 p1 = Point3(_lineSegments[i].point(0), 0.6f);
			Point3 p2 = Point3(_lineSegments[i].point(1), 0.6f);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			Draw::lineSegment(ls ,rd, Color4(1,1,1,.5));
			
			// Vector2 side = _lineSegments[i].point(0) - _lineSegments[i].point(1);
			// Vector2 normal = Vector2(-side.y, side.x); // always rotate 90 degrees CCW
			// p2 = p1 + Vector3(normal,0);
			// ls = LineSegment::fromTwoPoints(p1,p2);
			// Draw::lineSegment(ls ,rd);

		}
	} else if( renderQuality == HIGH_QUALITY) {
		// todo
	}
}

float FlatLens::timeToCollision(const Lasers::Laser& laserIn) {
	float ttc = -1;
	for (int i=0; i < _lineSegments.length(); i++) {
		float t = laserIn.ray.timeToIntersection(_lineSegments[i]);
		if ( ttc < 0 || (t < ttc && t > 0) ) {
			ttc = t;
		}
	}
	return ttc;
}

Array< Lasers::Laser > FlatLens::getResultingLasers(const Lasers::Laser& laserIn) {
	// refind first mirror side hit, could keep track of this in last call, but this is safer.
	float ttc = -1;
	int index = -1;
	for (int i=0; i < _lineSegments.length(); i++) {
		float t = laserIn.ray.timeToIntersection(_lineSegments[i]);
		if ( ttc < 0 || (t < ttc && t > 0) ) {
			ttc = t;
			index = i;
		}
	}

	Array< Lasers::Laser > resultList;
	
	Vector2 hitSide = _lineSegments[index].point(0) - _lineSegments[index].point(1);
	Vector2 normal = Vector2(-hitSide.y, hitSide.x); // always rotate 90 degrees CCW
	normal = normal.direction();

	// normal = Vector2(-1,-1).direction();
	// todo check normal side
	Point2 pos = laserIn.ray.point + laserIn.ray.direction * ttc;
	Lasers::Laser refractedRay;
	refractedRay.ray = laserIn.ray.getRefractedRay(pos, normal, 1.0f, Lasers::getIndexOfRefractionFromWavelength(laserIn.wavelength));
	refractedRay.ray.point = refractedRay.ray.getPointFromTime(RAY_NEAR_EPS);

	// todo better
	refractedRay.intensity = Lasers::intensityDecline(laserIn.intensity, ttc);
	refractedRay.wavelength = laserIn.wavelength;
	resultList.append(refractedRay);

	return resultList;
}

// Dolly& FlatLens::getDolly() {
// 	return _dolly;
// }
