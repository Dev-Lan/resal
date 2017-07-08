#include "Wall.hpp"


Wall::Wall() { }

Wall::Wall(const Array< LineSegment2D > lineSegments, const std::shared_ptr< Dolly >& dolly) : _initLineSegments(lineSegments) {
	_dolly = dolly;
	for (int i=0; i < lineSegments.length(); i++) {
		Matrix2 rotMat = dolly->getRotMat();
		Point2 p1 = rotMat * lineSegments[i].point(0) + Vector2(_dolly->getCenterPoint());
		Point2 p2 = rotMat * lineSegments[i].point(1) + Vector2(_dolly->getCenterPoint());
		_lineSegments.append(LineSegment2D::fromTwoPoints(p1,p2));
	}
}

Wall::~Wall() { }

// todo update for lists
Wall::Wall(const Wall& other) : _dolly(other._dolly) { }

// todo update for lists
Wall& Wall::operator=(const Wall& other) {
	_dolly = other._dolly;
	return *this;
}


void Wall::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	for (int i=0; i < _initLineSegments.length(); i++) {
		Matrix2 rotMat = _dolly->getRotMat();
		Point2 p1 = rotMat * _initLineSegments[i].point(0) + Vector2(_dolly->getCenterPoint());
		Point2 p2 = rotMat * _initLineSegments[i].point(1) + Vector2(_dolly->getCenterPoint());
		_lineSegments[i] = LineSegment2D::fromTwoPoints(p1,p2);
	}
}

void Wall::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	renderQuality = DEBUG_QUALITY; // TODO - remove after impementing high quality
	if (renderQuality == DEBUG_QUALITY) {
		for (int i=0; i < _lineSegments.length(); i++) {
			Point3 p1 = Point3(_lineSegments[i].point(0), 0.6f);
			Point3 p2 = Point3(_lineSegments[i].point(1), 0.6f);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			Draw::lineSegment(ls ,rd, Color3::black());
		}
	} else if( renderQuality == HIGH_QUALITY) {
		// todo
	}
}

float Wall::timeToCollision(const Lasers::Laser& laserIn) {
	float ttc = -1;
	for (int i=0; i < _lineSegments.length(); i++) {
		float t = laserIn.ray.timeToIntersection(_lineSegments[i]);
		if (ttc < 0 || (t < ttc && t > 0) ) {
			ttc = t;
		}
	}
	return ttc;
}

Array< Lasers::Laser > Wall::getResultingLasers(const Lasers::Laser& laserIn) {
	// this should only be called if the laser hits this wall, therefore there should be no resulting lasers
	Array< Lasers::Laser > emptyList;
	return emptyList;
}

// Dolly& Wall::getDolly() {
// 	return _dolly;
// }
