#include "Splitter.hpp"

Splitter::Splitter() { }

Splitter::Splitter(const Array< LineSegment2D > lineSegments, const std::shared_ptr< Dolly >& dolly) : _initLineSegments(lineSegments){
	_dolly = dolly;
	for (int i=0; i < lineSegments.length(); i++) {
		Matrix2 rotMat = dolly->getRotMat();
		Point2 p1 = rotMat * lineSegments[i].point(0) + Vector2(_dolly->getCenterPoint());
		Point2 p2 = rotMat * lineSegments[i].point(1) + Vector2(_dolly->getCenterPoint());
		_lineSegments.append(LineSegment2D::fromTwoPoints(p1,p2));
	}
}

Splitter::~Splitter() { }

// todo update for lists
Splitter::Splitter(const Splitter& other) : _dolly(other._dolly) { }

// todo update for lists
Splitter& Splitter::operator=(const Splitter& other) {
	_dolly = other._dolly;
	return *this;
}


void Splitter::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	// _dolly->onUserInput(ui, camera);
	for (int i=0; i < _initLineSegments.length(); i++) {
		Matrix2 rotMat = _dolly->getRotMat();
		Point2 p1 = rotMat * _initLineSegments[i].point(0) + Vector2(_dolly->getCenterPoint());
		Point2 p2 = rotMat * _initLineSegments[i].point(1) + Vector2(_dolly->getCenterPoint());
		_lineSegments[i] = LineSegment2D::fromTwoPoints(p1,p2);
	}
}

void Splitter::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	renderQuality = DEBUG_QUALITY; // TODO - remove after impementing high quality
	if (renderQuality == DEBUG_QUALITY) {
		glLineWidth(0.05);		
		for (int i=0; i < _lineSegments.length(); i++) {
			Point3 p1 = Point3(_lineSegments[i].point(0), 0.6f);
			Point3 p2 = Point3(_lineSegments[i].point(1), 0.6f);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			Draw::lineSegment(ls ,rd, Color3::white());
		}
		glLineWidth(1);	
	} else if( renderQuality == HIGH_QUALITY) {
		// todo
	}
}

float Splitter::timeToCollision(const Lasers::Laser& laserIn) {
	float ttc = -1;
	for (int i=0; i < _lineSegments.length(); i++) {
		float t = laserIn.ray.timeToIntersection(_lineSegments[i]);
		if ( ttc < 0 || (t < ttc && t > 0) ) {
			ttc = t;
		}
	}
	return ttc;
}

Array< Lasers::Laser > Splitter::getResultingLasers(const Lasers::Laser& laserIn) {
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
	Vector2 normal;

	Vector2 hitSide = _lineSegments[index].point(0) - _lineSegments[index].point(1);
	normal = Vector2(-hitSide.y, hitSide.x);
	normal = normal.direction();
	if (normal.dot(laserIn.ray.direction) > 0) {
		normal *= -1;
	}

	// normal = Vector2(-1,-1).direction();
	// todo check normal side
	Point2 pos = laserIn.ray.point + laserIn.ray.direction * ttc;
	Lasers::Laser reflectedLaser;
	reflectedLaser.ray = laserIn.ray.getReflectedRay(pos, normal);
	// todo better
	reflectedLaser.intensity = Lasers::intensityDecline(laserIn.intensity, ttc) / 2.f;
	reflectedLaser.wavelength = laserIn.wavelength;
	reflectedLaser.ray.point = reflectedLaser.ray.getPointFromTime(RAY_NEAR_EPS);
	resultList.append(reflectedLaser);

	Lasers::Laser throughLaser;
	throughLaser.ray = Ray2D(pos, laserIn.ray.direction);
	throughLaser.intensity = Lasers::intensityDecline(laserIn.intensity, ttc) / 2.f;
	throughLaser.wavelength = laserIn.wavelength;
	throughLaser.ray.point = throughLaser.ray.getPointFromTime(RAY_NEAR_EPS);
	resultList.append(throughLaser);

	return resultList;
}

// Dolly& Splitter::getDolly() {
// 	return _dolly;
// }
